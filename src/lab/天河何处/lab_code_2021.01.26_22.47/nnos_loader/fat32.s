# FAT32.ASM
# FAT32 Boot Sector
# Copyright (c) 1998, 2000, 2001, 2002 zhangjijun
# cc -m32 -c -o fat32.o fat32.s
# objcopy -O binary -j .text fat32.o fat32.bin
#.org 0x7c00

.intel_syntax
.globl start,main
.section .text
.code16    

start:
        jmp short main
        nop

OEMName:		    .ascii  "FrLdr1.0"
BytesPerSector:		.word   512
SectsPerCluster:	.byte   0x08
ReservedSectors:	.word   32
NumberOfFats:		.byte   2
MaxRootEntries:		.word   0			# Always zero for FAT32 volumes
TotalSectors:		.word   0			# Always zero for FAT32 volumes
MediaDescriptor:	.byte   0x0f8
SectorsPerFat:		.word   0			# Always zero for FAT32 volumes
SectorsPerTrack:	.word   0x03f
NumberOfHeads:		.word   255
HiddenSectors:   	.long   0x3f
TotalSectorsBig:	.long   4194241
# FAT32 Inserted Info
SectorsPerFatBig:	.long	4088
ExtendedFlags:	    .word	0
FSVersion:  		.word	0
RootDirStartCluster:.long	2
FSInfoSector:		.word	1
BackupBootSector:	.word	6
Reserved1:          .space  12
# End FAT32 Inserted Info
BootDrive:			.byte   0x80
Reserved:			.byte   0
ExtendSig:			.byte   0x29
SerialNumber:		.long   0x25D0A48E
VolumeLabel:		.ascii  "           "
FileSystem:			.ascii  "FAT32   "

main:
        xor ax,ax               # Setup segment registers
        mov ds,ax               # Make DS correct
        mov es,ax               # Make ES correct
        mov ss,ax				# Make SS correct
		mov bp,0x7c00
        mov sp,0x7c00           # Setup a stack


        cmpb byte ptr [bp+0x40],byte ptr 0xff
		#cmp BYTE ptr [bp+BootDrive],BYTE PTR 0xff	# If they have specified a boot drive then use it
		#.byte 0x80,0x7e,0x40,0xff
		jne CheckSectorsPerFat

        mov BYTE ptr [bp+0x40],dl				# Save the boot drive



CheckSectorsPerFat:
        cmp word ptr [bp+0x16],word ptr 0x00
        #cmp WORD ptr [bp+SectorsPerFat], word ptr 0x00	# Check the old 16-bit value of SectorsPerFat
		jnz CheckFailed								# If it is non-zero then exit with an error
CheckTotalSectors:	
		#.byte 0x66,0x83,0x7e,0x11,0x00						# Check the old 16-bit value of TotalSectors & MaxRootEntries
        #cmp	DWORD ptr [bp+MaxRootEntries],dword ptr 0x00# by comparing the DWORD at offset MaxRootEntries to zero
		cmp dword ptr [bp+0x11],dword ptr 0x00
		jnz CheckFailed								# If it is non-zero then exit with an error
CheckFileSystemVersion:
        cmp WORD ptr [bp+0x2a],word ptr 0x00		# Check the file system version word
		jna GetDriveParameters						# It is zero, so continue
CheckFailed:
        jmp PrintFileSystemError					# If it is not zero then exit with an error


GetDriveParameters:
		mov  ax,0x0800
		mov  dl,BYTE ptr [bp+0x40]					# Get boot drive in dl
		int  0x13									# Request drive parameters from the bios
		jnc  CalcDriveSize							# If the call succeeded then calculate the drive size

		# If we get here then the call to the BIOS failed
		# so just set CHS equal to the maximum addressable
		# size
		mov  cx,0x0ffff
		mov  dh,cl

CalcDriveSize:
		# Now that we have the drive geometry
		# lets calculate the drive size
		mov  bl,ch			# Put the low 8-bits of the cylinder count into BL
		mov  bh,cl			# Put the high 2-bits in BH
		shr  bh,6			# Shift them into position, now BX contains the cylinder count
		and  cl,0x3f		# Mask off cylinder bits from sector count
		# CL now contains sectors per track and DH contains head count
		movzx eax,dh		# Move the heads into EAX
		movzx ebx,bx		# Move the cylinders into EBX
		movzx ecx,cl		# Move the sectors per track into ECX
		inc   eax			# Make it one based because the bios returns it zero based
		inc   ebx			# Make the cylinder count one based also
		mul   ecx			# Multiply heads with the sectors per track, result in edx:eax
		mul   ebx			# Multiply the cylinders with (heads * sectors) [stored in edx:eax already]

		# We now have the total number of sectors as reported
		# by the bios in eax, so store it in our variable
		#mov   [BiosCHSDriveSize],eax
        #mov   [0x7da5],eax
        mov    [BiosCHSDriveSize+0x7c00],eax  		
        #.byte 0x66,0xa3,0xa5,0x7d
LoadExtraBootCode:
		# First we have to load our extra boot code at
		# sector 14 into memory at [0000:7e00h]
		#mov  eax,0eh
		mov  eax,0x03                            # 这个扇区号是以FAT32分区的逻辑1号扇区为基准的物理是66扇区，因为63是PBR，64号扇区是INFOFS扇区记录了分区可用的总蔟数 
        #add  eax,DWORD ptr [bp+HiddenSectors]	# Add the number of hidden sectors 
		#.byte 0x66,0x03,0x46,0x1c
		add eax,dword ptr [bp+0x1c]
		mov  cx,1
        xor  bx,bx
        mov  es,bx								# Read sector to [0000:7e00h]
		mov  bx,0x7e00
		call ReadSectors
		jmp  StartSearch



# Reads logical sectors into [ES:BX]
# EAX has logical sector number to read
# CX has number of sectors to read
ReadSectors:
		#cmp  eax,DWORD ptr [0x7da5]		# Check if they are reading a sector outside CHS range
		cmp  eax,dword ptr [BiosCHSDriveSize+0x7c00]
		jae  ReadSectorsLBA						# Yes - go to the LBA routine
												# If at all possible we want to use LBA routines because
												# They are optimized to read more than 1 sector per read

		pushad									# Save logical sector number & sector count

CheckInt13hExtensions:							# Now check if this computer supports extended reads
		mov  ah,0x41							# AH = 41h
		mov  bx,0x55aa							# BX = 55AAh
		mov  dl,BYTE ptr [bp+0x40]				# DL = drive (80h-FFh)
		int  0x13								# IBM/MS INT 13 Extensions - INSTALLATION CHECK
		jc   ReadSectorsCHS						# CF set on error (extensions not supported)
		cmp  bx,0xaa55							# BX = AA55h if installed
		jne  ReadSectorsCHS
		test cl,1								# CX = API subset support bitmap
		jz   ReadSectorsCHS						# Bit 0, extended disk access functions (AH=42h-44h,47h,48h) supported

		popad									# Restore sector count & logical sector number

ReadSectorsLBA:
		pushad									# Save logical sector number & sector count

		cmp  cx,64								# Since the LBA calls only support 0x7F sectors at a time we will limit ourselves to 64
		jbe  ReadSectorsSetupDiskAddressPacket	# If we are reading less than 65 sectors then just do the read
		mov  cx,64								# Otherwise read only 64 sectors on this loop iteration

ReadSectorsSetupDiskAddressPacket:
		#mov  [0x7d39],cx
		mov  [LBASectorsRead+0x7c00],cx
		push dword ptr 0
		push eax								# Put 64-bit logical block address on stack
		push es									# Put transfer segment on stack
		push bx									# Put transfer offset on stack
		push cx									# Set transfer count
		push byte ptr 0x10							# Set size of packet to 10h
		mov  si,sp								# Setup disk address packet on stack


        mov  dl,BYTE ptr [bp+0x40]				# Drive number
		mov  ah,0x42								# Int 13h, AH = 42h - Extended Read
		int  0x13								# Call BIOS
		jc   PrintDiskError						# If the read failed then abort
        #.byte 0x72,0x54    
		add sp, word ptr 0x10						# Remove disk address packet from stack

		popad									# Restore sector count & logical sector number

		push bx
		#mov  ebx,DWORD ptr [0x7d39]
		mov  ebx,dword ptr [LBASectorsRead+0x7c00]
        add  eax,ebx							# Increment sector to read
		shl  ebx,5
        mov  dx,es
        add  dx,bx								# Setup read buffer for next sector
        mov  es,dx
		pop  bx

		sub  cx,[LBASectorsRead+0x7c00]
        jnz  ReadSectorsLBA						# Read next sector

        retw

LBASectorsRead:
	   .long 0


# Reads logical sectors into [ES:BX]
# EAX has logical sector number to read
# CX has number of sectors to read
ReadSectorsCHS:
		popad										# Get logical sector number & sector count off stack

ReadSectorsCHSLoop:
        pushad
        xor   edx,edx
		#movzx ecx,WORD ptr [bp+SectorsPerTrack]
		#.byte 0x66,0x0f,0xb7,0x4e,0x18
		movzx ecx,word ptr [bp+0x18]
		div   ecx									# Divide logical by SectorsPerTrack
        inc   dl									# Sectors numbering starts at 1 not 0
		mov   cl,dl									# Sector in CL
		mov   edx,eax
		shr   edx,16
        #div   WORD ptr [bp+NumberOfHeads]			# Divide logical by number of heads
        #.byte 0xf7,0x76,0x1a
		div   word ptr [bp+0x1a]
		mov   dh,dl									# Head in DH
        mov   dl,BYTE ptr [bp+0x40]				# Drive number in DL
        mov   ch,al									# Cylinder in CX
        ror   ah,1									# Low 8 bits of cylinder in CH, high 2 bits
        ror   ah,1									#  in CL shifted to bits 6 & 7
        or    cl,ah									# Or with sector number
        mov   ax,0x0201
        int   0x13    # DISK - READ SECTORS INTO MEMORY
                     # AL = number of sectors to read, CH = track, CL = sector
                     # DH = head, DL    = drive, ES:BX -> buffer to fill
                     # Return: CF set on error, AH =    status (see AH=01h), AL    = number of sectors read

        jc    PrintDiskError						# If the read failed then abort

        popad

        inc   eax									# Increment Sector to Read

        mov   dx,es
        add   dx,word ptr 0x20							# Increment read buffer for next sector
        mov   es,dx

        loop  ReadSectorsCHSLoop					# Read next sector

        ret   




# Displays a disk error message
# And reboots
PrintDiskError:
        mov  si,offset msgDiskError+0x7c00			# Bad boot disk message
        call PutChars					# Display it

		jmp  Reboot

# Displays a file system error message
# And reboots
PrintFileSystemError:
        mov  si,offset msgFileSystemError+0x7c00		# FreeLdr not found message
        call PutChars					# Display it

Reboot:
        mov  si,offset msgAnyKey+0x7c00				# Press any key message
        call PutChars					# Display it
        xor ax,ax       
        int 0x16							# Wait for a keypress
        int 0x19							# Reboot

PutChars:
        lodsb
        or al,al
        jz short Done
        mov ah,0x0e
        mov bx,0x07
        int 0x10
        jmp short PutChars
Done:
        ret



BiosCHSDriveSize: .long 0

msgDiskError:		.ascii "Disk error\r\n\0"
msgFileSystemError:	.ascii "File system error\r\n\0"
msgAnyKey:			.ascii "Press any key to restart\r\n\0"

#        times 509-($-$$) db 0   # Pad to 509 bytes
#.space 24
.org 0x1fd
BootPartition:
		.byte 0

BootSignature:
        .word 0x0aa55       # BootSector signature
        

# End of bootsector
#
# Now starts the extra boot code that we will store
# at sector 14 on a FAT32 volume
#
# To remain multi-boot compatible with other operating
# systems we must not overwrite anything other than
# the bootsector which means we will have to use
# a different sector like 14 to store our extra boot code



StartSearch:
        # Now we must get the first cluster of the root directory
		#mov  eax,DWORD ptr [bp+RootDirStartCluster]
		mov  eax,DWORD ptr [bp+0x2c]
		cmp  eax,0x0ffffff8		# Check to see if this is the last cluster in the chain
		jb	 ContinueSearch		# If not continue, if so then we didn't find freeldr.sys
		jmp  PrintFileNotFound
ContinueSearch:
        mov  bx,0x2000
        mov  es,bx				# Read cluster to [2000:0000h]
        call ReadCluster        # Read the cluster


        # Now we have to find our way through the root directory to
        # The FREELDR.SYS file
		xor  bx,bx
        #mov  bl,BYTE ptr [bp+SectsPerCluster]
		mov  bl,byte ptr [bp+0x0d]
		shl  bx,4				# BX = BX * 512 / 32
        mov  ax,0x2000            # We loaded at 2000:0000
        mov  es,ax
        xor  di,di
        mov  si,offset filename+0x7c00
        mov  cx,11
        rep  cmpsb              # Compare filenames
        jz   FoundFile          # If same we found it
        dec  bx
        jnz  FindFile
        jmp  PrintFileNotFound

FindFile:
        mov  ax,es              # We didn't find it in the previous dir entry
        add  ax,2               # So lets move to the next one
        mov  es,ax              # And search again
        xor  di,di
        mov  si,offset filename+0x7c00        
        mov  cx,11
        rep  cmpsb              # Compare filenames
        jz   FoundFile          # If same we found it
        dec  bx                 # Keep searching till we run out of dir entries
        jnz  FindFile           # Last entry?

		# Get the next root dir cluster and try again until we run out of clusters
		#mov  eax,DWORD ptr [bp+RootDirStartCluster]
		mov   eax,dword ptr [bp+0x2c]
		call GetFatEntry
		#mov  dword ptr [bp+RootDirStartCluster],eax
        mov  dword ptr [bp+0x2c],eax
		jmp  StartSearch

FoundFile:

										# Display "Loading FreeLoader..." message
        mov  si,offset msgLoading+0x7c00				# Loading message
        call PutChars					# Display it

        xor  di,di						# ES:DI has dir entry
        xor  dx,dx
        mov  ax,WORD ptr es:[di+0x14]        # Get start cluster high word
		shl  eax,16
        mov  ax,WORD ptr es:[di+0x1a]        # Get start cluster low word

CheckStartCluster:
		cmp  eax,2						# Check and see if the start cluster starts at cluster 2 or above
		jnb  CheckEndCluster			# If so then continue
		jmp  PrintFileSystemError		# If not exit with error
CheckEndCluster:
		cmp  eax,0x0ffffff8				# Check and see if the start cluster is and end of cluster chain indicator
		jb   InitializeLoadSegment		# If not then continue
		jmp  PrintFileSystemError		# If so exit with error

InitializeLoadSegment:
        #mov  bx,0x800           # 根据系统的开始地址按实际地址填写      
        mov  bx,0x800            # 此处注意:muiltboot默认的启动地址是0000:8000H我们把启动地址定义在1000H处使1兆内的内存尽量连续
		mov  es,bx

LoadFile:
		cmp  eax,0x0ffffff8		# Check to see if this is the last cluster in the chain
		jae	 LoadFileDone		# If so continue, if not then read the next one
		push eax
        xor  bx,bx              # Load ROSLDR starting at 0000:1000h
		push es
		call ReadCluster
		pop  es

		xor  bx,bx
        #mov  bl,BYTE ptr [bp+SectsPerCluster]
		mov  bl,byte ptr [bp+0x0d]
		shl  bx,5				# BX = BX * 512 / 16
		mov  ax,es				# Increment the load address by
		add  ax,bx				# The size of a cluster
		mov  es,ax

		pop  eax
		push es
		call GetFatEntry		# Get the next entry
		pop  es

        jmp  LoadFile			# Load the next cluster (if any)

LoadFileDone:
        #mov  dl,BYTE ptr [bp+BootDrive]		# Load boot drive into DL
		mov  dl,byte ptr [bp+0x40] 
		#mov  dh,[BootPartition]			# Load boot partition into DH
        mov  dh,[BootPartition+0x7c00] 
		xor  ax,ax
        push ax					# We loaded at 0000:8000
        #push WORD ptr 0x8000	# We will do a far return to 0000:8000h
		push word ptr 0x08000
		#PUSH  WORD 0C200H      ；30天自制操作系统加载在0C200H,这个地址跟loader文件的ORG 后的地址对应
        retf                    # Transfer control to ROSLDR


# Returns the FAT entry for a given cluster number
# On entry EAX has cluster number
# On return EAX has FAT entry for that cluster
GetFatEntry:

		shl   eax,2								# EAX = EAX * 4 (since FAT32 entries are 4 bytes)
		mov   ecx,eax							# Save this for later in ECX
		xor   edx,edx
		#movzx ebx,WORD ptr [bp+BytesPerSector]
		movzx ebx,WORD ptr [bp+0x0b]
		push  ebx
		div   ebx								# FAT Sector Number = EAX / BytesPerSector
		#movzx ebx,WORD ptr [bp+ReservedSectors]
		movzx ebx,WORD ptr [bp+0x0e]
		add   eax,ebx							# FAT Sector Number += ReservedSectors
		#mov   ebx,DWORD ptr [bp+HiddenSectors]
		mov   ebx,dword ptr [bp+0x1c]
		add   eax,ebx							# FAT Sector Number += HiddenSectors
		pop   ebx
		dec   ebx
		and   ecx,ebx							# FAT Offset Within Sector = ECX % BytesPerSector
												# EAX holds logical FAT sector number
												# ECX holds FAT entry offset

												# Now we have to check the extended flags
												# to see which FAT is the active one
												# and use it, or if they are mirrored then
												# no worries
		#movzx ebx,WORD ptr [bp+ExtendedFlags]	# Get extended flags and put into ebx
		movzx ebx,word ptr [bp+0x28]
		and   bx,0x0f							# Mask off upper 8 bits, now we have active fat in bl
		jz    LoadFatSector						# If fat is mirrored then skip fat calcs
		#cmp   bl,BYTE ptr [bp+NumberOfFats]			# Compare bl to number of fats
		cmp   bl,byte ptr [bp+0x10]
		jb    GetActiveFatOffset
		jmp   PrintFileSystemError				# If bl is bigger than numfats exit with error
GetActiveFatOffset:
		push  eax								# Save logical FAT sector number
		#mov   eax,dword ptr [bp+SectorsPerFatBig]	# Get the number of sectors occupied by one fat in eax
		mov   eax,dword ptr [bp+0x24]
		mul   ebx								# Multiplied by the active FAT index we have in ebx
		pop   edx								# Get logical FAT sector number
		add   eax,edx							# Add the current FAT sector offset

LoadFatSector:
		push  ecx
		# EAX holds logical FAT sector number
		# Check if we have already loaded it
		#cmp  eax,DWORD ptr [FatSectorInCache]
		cmp  eax,dword ptr [FatSectorInCache+0x7c00]
		je   LoadFatSectorAlreadyLoaded

		mov  DWORD ptr [FatSectorInCache+0x7c00],eax
        mov  bx,0x7000
        mov  es,bx
        xor  bx,bx								# We will load it to [7000:0000h]
		mov  cx,1
		call ReadSectors

LoadFatSectorAlreadyLoaded:
        mov  bx,0x7000
        mov  es,bx
		pop  ecx
		mov  eax,DWORD ptr es:[ecx]					# Get FAT entry
		and  eax,0x0fffffff						# Mask off reserved bits

		ret

FatSectorInCache:								# This variable tells us which sector we currently have in memory
	.long	0x0ffffffff								# There is no need to re-read the same sector if we don't have to


# Reads cluster number in EAX into [ES:0000]
ReadCluster:
		# StartSector = ((Cluster - 2) * SectorsPerCluster) + ReservedSectors + HiddenSectors#

		dec   eax
		dec   eax
		xor   edx,edx
		#movzx ebx,BYTE ptr [bp+SectsPerCluster]
		movzx ebx,BYTE ptr [bp+0x0d]
		mul   ebx
		push  eax
		xor   edx,edx
		#movzx eax,BYTE ptr [bp+NumberOfFats]
		movzx eax,byte ptr [bp+0x10]
		#mul   DWORD ptr [bp+SectorsPerFatBig]
		mul   DWORD ptr [bp+0x24]
		#movzx ebx,WORD ptr [ bp+ReservedSectors]
		movzx ebx,WORD ptr [ bp+0x0e]
		add   eax,ebx
		#add   eax,DWORD ptr [ bp+HiddenSectors]
		add   eax,dword ptr [bp+0x1c]
		pop   ebx
		add   eax,ebx			# EAX now contains the logical sector number of the cluster
        xor   bx,bx				# We will load it to [ES:0000], ES loaded before function call
		#movzx cx,BYTE ptr [ bp+SectsPerCluster]
		movzx cx,byte ptr [bp+0x0d]
		call  ReadSectors
		ret


# Displays a file not found error message
# And reboots
PrintFileNotFound:
        mov  si,offset msgFreeLdr+0x7c00      # FreeLdr not found message
        call PutChars           # Display it
        mov  si,offset msgAnyKey+0x7c00       # Press any key message
        call PutChars           # Display it

		jmp  Reboot

msgFreeLdr:   .ascii "freeldr.sys not found\r\n\0"
filename:     .ascii "FREELDR SYS"
msgLoading:   .ascii "Loading FreeLoader...\r\n\0"


#        times 1022-($-$$) db 0   # Pad to 1022 bytes
#.space  67
 .org  1022
 .word 0x0aa55      # BootSector signature
