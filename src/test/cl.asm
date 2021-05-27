.486p
         org       900h
code     segment   public  use16
         assume    cs:code,ds:code,es:code
        
_startup:
         jmp       short init        
                                          
gdt:
dummy:   dd        00000000h
         dd        00000000h
code32:  dd        0000ffffh
         dd        00cf9800h
data32:  dd        0000ffffh
         dd        00cf9200h
video:   dd        8000ffffh
         dd        0040920bh   
GdtPtr   dw        $-gdt-1
         dd        offset gdt+900h 
         ;dd        0
;----------------------------------------------------------
init:
         mov       ax,cs
         mov       ds,ax
         mov       es,ax
         mov       ss,ax
         mov       sp,100h
;-----------------------------------------------------------
         xor       eax,eax
         mov       ax ,cs         
         shl       eax, 4
         ;add       eax, offset flush +900h +0a0h
         add       eax,offset flush +900h
         mov       word PTR [code32 +900h + 2], ax
         shr       eax, 16
         mov       byte PTR [code32 +900h + 4], al
         mov       byte PTR [code32 +900h + 7], ah
;------------------------------------------------------------
         xor       eax, eax
         mov       ax,  ds
         shl       eax, 4
         add       eax, offset data32 +900h
         mov       word ptr [data32 +900h +2],ax
         shr       eax,16
         mov       byte ptr [data32 +900h +4],al
         mov       byte ptr [data32 +900h +7],ah
;--------------------------------------------------------------
         ;xor       eax,eax
         ;mov       ax, ss        
         ;shl       eax,4
         ;add       eax,offset stack32
         ;mov       word ptr [stack32 +2],ax
         ;shr       eax,16
         ;mov       byte ptr [stack32 +4],al
         ;mov       byte ptr [stack32 +7],ah
;----------------------------------------------------------------
         xor       eax, eax
         mov       ax, ds
         shl       eax, 4
         add       ax, offset gdt+900h
         mov       dword PTR [GdtPtr + 902h], eax
         lgdt      fword ptr [GdtPtr + 900h]
;---------------------------------------------------------------
         cli
;---------------------------------------------------------------
         in        al,92h
         or        al,00000010b
         out       92h,al
;---------------------------------------------------------------
         mov       eax,cr0
         or        eax,1
         mov       cr0,eax
;---------------------------------------------------------------
         db        66h
         db        0eah
         dd        offset flush-1
         ;dd        offset flush
         dw        0008h
         db 66h
;---------------------------------------------------------------
;code     ends
        
;cseg     segment   public   use32
;         assume    cs:cseg
flush:          
         mov       ax,0010h
         mov       ds,ax
         mov       ax,0018h
         mov       es,ax          
         push      ebx
         mov       ebx,0
         add       ebx,0
         mov       edi,ebx
         ;mov       dword ptr es:[edi+0A0h],0c48h
         mov       dword ptr ds:[edi+0b8000h+0a0h],0c48h
         mov       dword ptr es:[edi+0A2h],0c49h
         pop       ebx
         xor       ecx,ecx
next:
         inc       ecx
         xor       ebx,ebx
         add       ebx,ecx
         add       ebx,ecx
         mov       edi,ebx
         mov       al, byte ptr cs:[MsgBegin+ecx-1]
         ;mov       al,byte ptr cs:[MsgBegin+bx+di-1]
         ;add       [si+bx],al
         mov       ah,0ch
         mov       dword ptr ds:[0b8000h+edi+0A6h],eax
         cmp       ecx,MsgEnd-MsgBegin
         jnz       next
         ;jmp       $
         ;mov       eax,9h        ;kernel.bin所在的扇区号 0x9
         ;mov       ebx,70000h    ;写入的内存地址 0x70000
         ;mov       ecx,200        ;读入的扇区数
         ;call      rd_disk_m_32
         ;call      kernel_init
         mov       byte ptr es:[280h],'i'
         mov       byte ptr es:[282h],'n'
         mov       byte ptr es:[284h],'i'
         mov       byte ptr es:[286h],'t'
         mov       byte ptr es:[28ah],'k'
         mov       byte ptr es:[28ch],'e'
         mov       byte ptr es:[28eh],'r'
         mov       byte ptr es:[290h],'n'
         mov       byte ptr es:[292h],'e'
         mov       byte ptr es:[294h],'l'

         ;mov       esp,0c009f000h
         ;jmp       0c0001500h
         ;db        0e9h
         ;dd        0c0001500h
         ;jmp       near ptr bootpack+24h+0c0h
         jmp       short bootpack+24h
;-------------------------------------------------------------------------------		 
         
MsgBegin db        'Hello World',0
MsgEnd   equ       $
Row      dw        80 * 11 * 2
Line     dw        (79 - (MsgEnd - MsgBegin)) * 2
;---------------------------------------------------------------------------
bootpack:
code     ends          
;cseg     ends
         end       _startup
