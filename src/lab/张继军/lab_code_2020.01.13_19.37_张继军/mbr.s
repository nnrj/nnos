#=====================================================================================
#
# MBR ( Master Boot Record )
#
# Author : zhangjijun
# History:
# 0.01 - 2005-08-13 19:34 ���ô�ͳ��CHS���̵���
# 0.02 - 2005-08-23 19:44 ���Ȳ�����չ���̵���
#
# Build :   as --32 mbr.s -o mbr.o
#           objcopy -O binary -j .text mbr.o mbr.bin
#  
#====================================================================================
#
# MBR( Master Boot Record )��������¼���������ֵ����ݣ�ǰ446�ֽ�Ϊ�������뼰���ݣ���
# ��446(0x1BE)��ʼ���Ƿ��������������ĸ���������ɣ�ÿ������������Ϊ16�ֽڣ���¼��
# ����ʱ��Ҫ�ķ���������
#
# ��CPU�ϵ�֮������Ӳ����������BIOS��Ӳ�̵���������¼��λ��0���桢0�ŵ���1��������
# ��7C00����Ȼ�󽫿���Ȩ�������������롣��������������������
# (1) ɨ��������ҵ�һ������(������)������
# (2) �ҵ������������ʼ������
# (3) �������������������װ�ص��ڴ�7C00����
# (4) ������Ȩ���������������룻
#  
# ��������������޷������������������ʾ���´�����Ϣ֮һ��
# No active partition.
# Invalid partition table.
# Error loading operating system.
# Missing operating system.
#
#====================================================================================
# FAT16�����ߴ���LBA
#====================================================================================
# LBA   HeadsPerCylinder SectorsPerTrack  Maximum Size for Boot Partition
# Disabled 64     32     1GB
# Enabled  255     63     4GB
#
# Ϊ����Ӧ����8G��Ӳ�̣�Windows2000������Start CHS��End CHS����ʹ��StartLBA��TotalSector  
# ��ȷ�����������������е�λ�úʹ�С��
#  
#====================================================================================
#     ��������ṹ(16�ֽ�)
#====================================================================================
#
# typedef struct _PARTITION_ENTRY
# {
#  UCHAR BootIndicator#  // �ܷ�������־
#  UCHAR StartHead#   // �÷�����ʼ��ͷ��
#  UCHAR StartSector#  // ��ʼ����Ÿ�2λ��6λ��ʼ������
#  UCHAR StartCylinder#  // ��ʼ����ŵ�8λ
#  UCHAR PartitionType#  // ��������
#  UCHAR EndHead#   // �÷�����ֹ��ͷ��
#  UCHAR EndSector#   // ��ֹ����Ÿ�2λ��6λ��ֹ������
#  UCHAR EndCylinder#  // ��ֹ����ŵ�8λ
#  ULONG StartLBA#   // ��ʼ������
#  ULONG TotalSector#  // �����ߴ磨����������
# }PARTITION_ENTRY,*PPARTITION_ENTRY#
#
#====================================================================================
#    ��������¼(MBR)�ṹ
#====================================================================================
# typedef struct _MASTER_BOOT_RECORD
# {
#  UCHAR    BootCode[446]#
#  PARTITION_ENTRY  Partition[4]#
#  USHORT    Signature#
# }MASTER_BOOT_RECORD,*PMASTER_BOOT_RECORD#
#
#====================================================================================
.intel_syntax
.globl _ENTRY_POINT
.section .text
.code16                        # ����16λ���������32λ����
#.org                   0x0600  # ָ������װ���ڴ����ʼλ��
#=========================================================================
#
# ��ͳ�������
#
#====================================================================

.set ACTIVE_FLAG     ,  0x80    # ����(������)������־
.set NOT_ACTIVE_FLAG ,  0x00    # �������־
.set MBR_MOVE_ADDR   ,  0x0600  # MBR���ƶ�������λ��Ȼ��������
.set BOOT_SIGNATURE  ,  0xAA55  # ������־
.set SEC_SIG_OFF     ,  0x01FE  # ���������ı�־λ��

#====================================================================
# ��������ṹƫ��
#====================================================================
.set BootIndicator   ,  0       # �ܷ�������־
.set StartHead       ,  1       # �÷�����ʼ��ͷ��
.set StartSector     ,  2       # ��ʼ����Ÿ�2λ��6λ��ʼ������
.set StartCylinder   ,  3       # ��ʼ����ŵ�8λ
.set PartitionType   ,  4       # ��������
.set EndHead         ,  5       # �÷�����ֹ��ͷ��
.set EndSector       ,  6       # ��ֹ����Ÿ�2λ��6λ��ֹ������
.set EndCylinder     ,  7       # ��ֹ����ŵ�8λ
.set StartLBA        ,  8       # ��ʼ������
.set TotalSector     ,  12      # �����ߴ磨����������

#====================================================================
# ���õķ�������
#====================================================================
.set PA_EMPTY        ,  0x00    # �շ���
.set PA_FAT12        ,  0x01    # FAT12 ( < 32680 sectors )
.set PA_FAT16        ,  0x04    # FAT16 ( 32680 - 65535 sectors )
.set PA_EXTENDED     ,  0x05    # DOS EXTENDED
.set PA_BIGDOS_FAT16 ,  0x06    # FAT16 ( 33MB - 4GB )
.set PA_NTFS         ,  0x07    # NTFS
.set PA_FAT32        ,  0x0B    # FAT32
.set PA_FAT32_LBA    ,  0x0C    # FAT32 LBA
.set PA_FAT16_LBA    ,  0x0E    # FAT16 LBA
.set PA_EXTENDED_LBA ,  0x0F    # LBA EXTENDED
.set PA_DYNAMIC_DISK ,  0x42    # Dynamic Disk Volume
#====================================================================
# ��������¼�����
#====================================================================
_ENTRY_POINT:                   # ��ʼ����ؼĴ�������־λ
         CLI                    # �ȹص��ж�
         CLD                    # ����Ϊ��ǰ����
         XOR    AX,AX           # AX = 0
         MOV    DS,AX           # �������ݶμĴ��� DS:SI
         MOV    ES,AX           # ���ø��ӶμĴ��� ES:DI
         MOV    SS,AX           # ���ö�ջ�μĴ���
         MOV    BP,0x7C00       # ���û�ַ�Ĵ���
         MOV    SP,BP           # ���ö�ջջ��
  # ��MBR�����ƶ���0600H��
         MOV    SI,BP           # SI = 7C00H
         MOV    DI,0x600        # DI = 0600H
         MOV    CX,512          # ���ƶ����ֽ���
         REP    MOVSB 
         JMP    0:.RealStart
# ������ʼ   
.RealStart:
 # ����������������
         MOV    byte ptr [ DriveNumber ] , DL
 #====================================================================  
 # ����Ƿ�֧�ִ����ж�INT 13H����չ
 #====================================================================
         MOV    AH,0x41
         MOV    BX,0x055AA
         INT    0x13
         JC     .LookupActive                            # ���ʧ�ܣ���λ��־Ϊ1
         MOV    BYTE ptr [DiskExtension],0x01            # ����֧�ִ�����չ��־
.LookupActive:
 # ���Ҽ������
         MOV    BP,offset PartitionTable                 # ָ�������
         MOV    BL,4                                     # ���4������
#��鼤����� 
.CheckNext: 
         CMP    BYTE ptr [BP+BootIndicator],ACTIVE_FLAG  # ���÷����Ƿ񼤻�
         JZ     .FoundActive                             # �ҵ��������
         CMP    BYTE ptr [BP+BootIndicator],NOT_ACTIVE_FLAG  # ���÷����Ƿ񼤻� 
         JNZ    .InvalidTable                            # ��Чֵ
         ADD    BP,0x10                                   # ָ����һ����������
         DEC    BL
         JZ     .NoActive                                # û���ҵ��������
         JMP    .CheckNext
#�ҵ��˼������
.FoundActive:
 # �����������
         MOV    AL,4
         SUB    AL,BL                                   # AL = 4-BL = ��һ�����������������(0-3)
         MOV    BYTE ptr [ActivePartition],AL               # ���漤�������������
         MOV    DI,BP                                   # DI = ���������
         MOV    DH,BYTE ptr [BP+StartHead]                   # �÷�����ʼ��ͷ��
         MOV    CL,BYTE ptr [BP+StartSector]                 # ��ʼ����Ÿ�2λ��6λ��ʼ������
         MOV    CH,BYTE ptr [BP+StartCylinder]               # ��ʼ����ŵ�8λ
  # ������ʼ������
         MOV    AX,WORD ptr [BP+StartLBA+2]
         MOV    WORD ptr [DAP_SECTOR_LOW+2],AX 
         MOV    AX,WORD ptr[BP+StartLBA]  
         MOV    WORD ptr [DAP_SECTOR_LOW],AX 
 # ���ȷ��ֻ��һ��������� 
.Recheck:     
         ADD    BP,0x10                                  # ָ����һ��������
         DEC    BL
         JZ     .LoadBootSector                          # װ�ظ÷�������������
         CMP    BYTE ptr [BP+BootIndicator],NOT_ACTIVE_FLAG # ���÷����Ƿ񼤻� 
         JNZ    .InvalidTable                           # ��Чֵ
         JMP    .Recheck
# װ�ؼ���������������� 
.LoadBootSector:
  # ������������
         MOV    DL,BYTE ptr [DriveNumber]
  # ����Ƿ�֧����չ���̵���
         CMP    BYTE ptr [DiskExtension],0x01
         JNZ    .NoDiskExtension
  # ʹ����չ���̵��ö�ȡ��������
 # 
 # INT 13H 
 #  AH = 42H
 #  DL = Drive Number
 # DS:SI = ָ����̵�ַ����ָ��
 #
         MOV    SI,offset DAP_PACKET_SIZE
         MOV    AH,0x42
         INT    0x13
         JC     .ErrorLoadOS
         JMP    .CheckBootSector
.NoDiskExtension: 
 #====================================================================
 # 
 # INT 13H 
 #  AH = 2        ����ţ�0 - 1023
 #  AL = Ҫ��ȡ��������     ��ͷ�ţ�0 - 255   
 #  CH = ����ŵ�8λ     �����ţ�1 - 63 
 #  CL = ����Ÿ�2λ : 6λ������  
 #  DH = ��ͷ��
 #  DL = ��������
 #  ES:BX = ������
 #
 #====================================================================
 # ��ȡ�������� 
         MOV    BX,0x7C00
         MOV    AX,0x0201
         INT    0x13
         JC     .ErrorLoadOS
 # ������������Ƿ�Ϸ� 
.CheckBootSector: 
 # װ�����������ɹ������������־
         MOV    BX,0x7C00
         CMP    WORD ptr [BX+SEC_SIG_OFF], BOOT_SIGNATURE  # ���������־
         JNZ    .MissingOS     
 # ׼����ת������������������������
 # DL = ������������
 # DH = ���������
 # DI = ���������
         MOV    DL,BYTE ptr [DriveNumber]
         MOV    DH,BYTE ptr [ActivePartition]
         JMP    0:0x7C00
# û�м������
.NoActive: 
         MOV    SI,offset MsgNoActive
         CALL   ShowMessage
         JMP    .Hang
 # ��Ч������
.InvalidTable:
         MOV    SI,offset MsgPartitionTable
         CALL   ShowMessage
         JMP    .Hang
 # װ����������ʧ��
.ErrorLoadOS:
         MOV    SI,offset MsgLoadingOS
         CALL   ShowMessage
         JMP    .Hang
 # �����������Ϸ�
.MissingOS:
         MOV    SI,offset MsgMissingOS
         CALL   ShowMessage
.Hang:
         JMP    .Hang		 
#====================================================================
#
# ��ʾһ���ַ���
# ���룺
#   DS:SI  = �ַ�������ʼ��ַ(��NULL����)
#   
#====================================================================
ShowMessage:
         LODSB      # AL = DS:[SI] SI = SI+1
         OR     AL,AL  # ����Ƿ�����NULL�ַ���
         JZ     .ShowEnd
         MOV    AH,0x0E
         MOV    BX,0x07
         INT    0x10
         JMP    ShowMessage
.ShowEnd:
         RET 
#====================================================================
# ��������
#====================================================================
.ifdef  DEBUG
#====================================================================
#
# ��ʾһ���ַ�
# ���룺 AL = ����ʾ�ַ�
#
#====================================================================
PrintChar:
         PUSH   AX
         PUSH   BX
         MOV    AH,0x0E
         MOV    BX,7
         INT    0x10
         POP    BX
         POP    AX
         RET
#====================================================================
#
# ��ʾ16���Ƶ�ֵ(��һ��BYTE��Ϊ����ASCII�ַ���ӡ���������ڵ���)
# ���룺 AL = ����ʾ���ֽ�
#
#====================================================================
PrintByte:
         PUSH   BX
         MOV    BH,AL
  # ��ʾ��4λ
         SHR    AL,4
         AND    AL,0x0F
         ADD    AL,0x30
         CMP    AL,0x39
         JLE    .PrintIt
         ADD    AL,0x07
.PrintIt: 
         CALL   PrintChar
 # ��ʾ��4λ 
         MOV    AL,BH
         AND    AL,0x0F
         ADD    AL,0x30
         CMP    AL,0x39
         JLE    .PrintItAgain
         ADD    AL,0x07        
.PrintItAgain: 
         CALL   PrintChar
         POP    BX
         RET
.endif  #DEBUG
#====================================================================
# ������
#====================================================================
MsgNoActive:         .ascii "No active partition.\0"
MsgPartitionTable:   .ascii "Invalid partition table.\0"   
MsgLoadingOS:        .ascii "Error loading operating system.\0"
MsgMissingOS:        .ascii "Missing operating system.\0"

#====================================================================
#��ʱ����
#====================================================================
DriveNumber:         .byte  0x80   # ����������������
ActivePartition:     .byte  0x80   # �������������(0-3) 
DiskExtension:       .byte  0x00   # �Ƿ�֧�ִ�����չ����
 
#====================================================================  
# ��չ���̷�����ʹ�õĵ�ַ��
#====================================================================
DAP_PACKET_SIZE:     .byte  0x10   # ���Ĵ�СΪ16�ֽ�
DAP_RESERVED1:       .byte  0x00   # �����ֽ�
DAP_READ_SECTORS:    .byte  0x01   # Ҫ�����������(1 - 127 )
DAP_RESERVED2:       .byte  0x00   # �����ֽ�
DAP_BUFFER_OFF:      .word  0x7C00 # ������ƫ��
DAP_BUFFER_SEG:      .word  0x0000 # �������ε�ַ
DAP_SECTOR_LOW:      .long  0x0000 # ��ʼ�����ŵĵ�32λ
DAP_SECTOR_HIGH:     .long  0x0000 # ��ʼ�����ŵĸ�32λ
#====================================================================
# ����ֽ�
.org  0x1b8           
#                     .space 95    
#====================================================================

#====================================================================
# ��־�ֽ�
#====================================================================
UniqueMbrSignature:  .long  0x4B43414A
UnknownWord:         .word  0x00   
#====================================================================
# ������(ƫ��Ϊ446)
#====================================================================
PartitionTable:      .byte  0x80
                     .byte  0x01
                     .byte  0x01
                     .byte  0x00
                     .byte  0x0C
                     .byte  0x0FE
                     .byte  0x0BF
                     .byte  0x0FC
                     .long  0x0000003F
                     .long  4194241 
                     .space 48  
#====================================================================
# �������ı���ֽ�(NASM��֧���ظ�ORG)
#====================================================================
BootSignature:       .word 0xAA55
#====================================================================
# �������
#====================================================================

  
 
 

 


