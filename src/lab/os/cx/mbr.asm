NUMsector      EQU    6       ; 设置读取到的软盘最大扇区编号()
NUMheader      EQU    0        ; 设置读取到的软盘最大磁头编号(01)
NUMcylind      EQU    0        ; 设置读取到的软盘柱面编号

mbrseg         equ    7c0h     ;启动扇区存放段地址
newseg         equ    800h     ;跳出MBR之后的新段地址
dptseg         equ    7e0h     ;DPT区段地址

jmp   start
msgwelcome:     db '------Wlecome Jiang Os------','$'
msgstep1:       db 'Step1:now  is   in   mbr','$'
msgmem1:        db 'Memory Address is---','$'
msgcs1:         db 'CS:????H','$'

cylind  db 'Cylind:?? $',0    ; 设置开始读取的柱面编号
header  db 'Header:?? $',0    ; 设置开始读取的磁头编号
sector  db 'Sector:?? $',2    ; 设置从第2扇区开始读
FloppyOK db 'Read OK','$'
Fyerror db 'Read Error' ,'$'

start:

call  inmbr
call  floppyload
jmp   newseg:0        ;通过此条指令跳出MBR区


inmbr:
mov   ax,mbrseg   ;为显示各种提示信息做准备
mov   ds,ax
mov   ax,newseg
mov   es,ax   ;为读软盘数据到内存做准备，因为读软盘需地址控制---ES:BX
call  inmbrshow
call  showcs
call  newline
call  newline
call  newline
ret



inmbrshow:
mov   si,msgwelcome
call  printstr
call  newline
call  newline
mov   si,msgstep1
call  printstr
call  newline
mov   si,msgmem1
call  printstr
ret


printstr:                  ;显示指定的字符串, 以'$'为结束标记
      mov al,[si]
      cmp al,'$'
      je disover
      mov ah,0eh
      int 10h
      inc si
      jmp printstr
disover:
      ret

newline:                     ;显示回车换行
      mov ah,0eh
      mov al,0dh
      int 10h
      mov al,0ah
      int 10h
      ret


showcs:                       ;展示CS的值
      mov  ax,cs

      mov  dl,ah
      call HL4BIT
      mov  dl,  BH
      call  ASCII
      mov  [msgcs1+3],dl

      mov  dl,  Bl
      call  ASCII
      mov  [msgcs1+4],dl

      mov  dl,al
      call HL4BIT
      mov  dl,  BH
      call  ASCII
      mov  [msgcs1+5],dl

      mov  dl,  Bl
      call  ASCII
      mov  [msgcs1+6],dl

      mov  si,  msgcs1
      call printstr

      ret


;-----------------将16进制数字(1位)转换成ASCII码，输入DL,输出DL------
ASCII:   CMP  DL,9
         JG   LETTER  ;DL>OAH
         ADD  DL,30H  ;如果是数字,加30H即转换成ASCII码
         RET
LETTER:  ADD  DL,37H  ;如果是A～F,加37H即转换成ASCII码
         RET

;-----------------取出1个字节Byte的高4位和低4位,输入DL,输出BH和BL----------

HL4BIT:  MOV  DH,dl
         MOV  BL,dl
         SHR  DH,1
         SHR  DH,1
         SHR  DH,1
         SHR  DH,1
         MOV  BH,DH                    ;取高4位
         AND  BL,0FH                   ;取低4位
         RET


floppyload:
     call    read1sector
     MOV     AX,ES
     ADD     AX,0x0020
     MOV     ES,AX                ;一个扇区占512B=200H，刚好能被整除成完整的段,因此只需改变ES值，无需改变BP即可。
     inc   byte [sector+11]
     cmp   byte [sector+11],NUMsector+1
     jne   floppyload             ;读完一个扇区
     mov   byte [sector+11],1
     inc   byte [header+11]
     cmp   byte [header+11],NUMheader+1
     jne   floppyload             ;读完一个磁头
     mov   byte [header+11],0
     inc   byte [cylind+11]
     cmp   byte [cylind+11],NUMcylind+1
     jne   floppyload             ;读完一个柱面

     ret


numtoascii:     ;将2位数的10进制数分解成ASII码才能正常显示。如柱面56 分解成出口ascii: al:35,ah:36
     mov ax,0
     mov al,cl  ;输入cl
     mov bl,10
     div bl
     add ax,3030h
     ret

readinfo:       ;显示当前读到哪个扇区、哪个磁头、哪个柱面
     mov si,cylind
     call  printstr
     mov si,header
     call  printstr
     mov si,sector
     call  printstr
     ret



read1sector:                      ;读取一个扇区的通用程序。扇区参数由 sector header  cylind控制

       mov   cl, [sector+11]      ;为了能实时显示读到的物理位置
       call  numtoascii
       mov   [sector+7],al
       mov   [sector+8],ah

       mov   cl,[header+11]
       call  numtoascii
       mov   [header+7],al
       mov   [header+8],ah

       mov   cl,[cylind+11]
       call  numtoascii
       mov   [cylind+7],al
       mov   [cylind+8],ah

       MOV        CH,[cylind+11]    ; 柱面从0开始读
       MOV        DH,[header+11]    ; 磁头从0开始读
       mov        cl,[sector+11]    ; 扇区从1开始读

        call       readinfo        ;显示软盘读到的物理位置
        mov        di,0
retry:
        MOV        AH,02H            ; AH=0x02 : AH设置为0x02表示读取磁盘
        MOV        AL,1            ; 要读取的扇区数
        mov        BX,    0         ; ES:BX表示读到内存的地址 0x0800*16 + 0 = 0x8000
        MOV        DL,00H           ; 驱动器号，0表示第一个软盘，是的，软盘。。硬盘C:80H C 硬盘D:81H
        INT        13H               ; 调用BIOS 13号中断，磁盘相关功能
        JNC        READOK           ; 未出错则跳转到READOK，出错的话则会使EFLAGS寄存器的CF位置1
           inc     di
           MOV     AH,0x00
           MOV     DL,0x00         ; A驱动器
           INT     0x13            ; 重置驱动器
           cmp     di, 5           ; 软盘很脆弱，同一扇区如果重读5次都失败就放弃
           jne     retry

           mov     si, Fyerror
           call    printstr
           call    newline
           jmp     exitread
READOK:    mov     si, FloppyOK
           call    printstr
           call    newline
exitread:
           ret


times 510-($-$$) db 0
db 0x55,0xaa

;-------------------------------------------------------------------------------
;------------------此为扇区分界线，线上为第1扇区，线下为第2扇区-----------------
;-------------------------------------------------------------------------------

jmp    newprogram


gdt_size dw 32-1     ;GDT 表的大小 ;（总字节数减一）
gdt_base dd 0x00007e00 ;GDT的物理地址

msgstep2:       db 'Step2:now  jmp  out  mbr','$'
mesmem2:        db 'Will Visit Memory Address is---','$'
msgcs2:         db '0X:????(CS):XXXX','$'

msgstep3:       db 'Step3:now  enter  protect  mode','$'

newprogram:
mov     ax,newseg      ;跳转到新地址8000H之后，全部寄存器启用新的段地址
sub     ax,20h         ;要调整一下DS的值才能正确访问新程序中的数据
mov     ds,ax          ;ds用于打印数据的寻址

call  outmbr
call  showcsnew

call  showprotect



mov     ax,dptseg
mov     es,ax          ;es用于gpt区寻址    gpt存放起始地址:0x00007e00h
call    createdpt

jmp  next





;创建DPT子程序
createdpt:

lgdt [gdt_size] ;将DPT的地址和大小写入gdtr生效     默认DS

;创建0#描述符，它是空描述符，这是处理器的要求
mov dword [es:0x00],0x00
mov dword [es:0x04],0x00

;创建#1描述符，保护模式下的代码段描述符
mov dword [es:0x08],0x8000ffff
mov dword [es:0x0c],0x00409800

;创建#2描述符，保护模式下的数据段描述符（文本模式下的显示缓冲区）
;mov dword [es:0x10],0x8000ffff
;mov dword [es:0x14],0x0040920b

mov dword [es:0x10],0x0000ffff  ;（把DS的基地址定义为0）
mov dword [es:0x14],0x00c09200  ; (标志位G=1,表示以KB为单位)

;创建#3描述符，保护模式下的堆栈段描述符
mov dword [es:0x18],0x00007a00
mov dword [es:0x1c],0x00409600
ret





outmbr:
call  newlinenew
call  newlinenew
mov   si,msgstep2
call  printstrnew
call  newlinenew
mov   si,mesmem2
call  printstrnew
ret


showprotect:
call  newlinenew
call  newlinenew
mov   si,msgstep3
call  newlinenew
call  printstrnew
call  newlinenew
ret


showcsnew:                       ;展示CS的值
      mov  ax,cs

      mov  dl,ah
      call HL4BITnew
      mov  dl,  BH
      call  ASCIInew
      mov  [msgcs2+3],dl

      mov  dl,  Bl
      call  ASCIInew
      mov  [msgcs2+4],dl

      mov  dl,al
      call HL4BITnew
      mov  dl,  BH
      call  ASCIInew
      mov  [msgcs2+5],dl

      mov  dl,  Bl
      call  ASCIInew
      mov  [msgcs2+6],dl

      mov  si,  msgcs2
      call printstrnew

      ret

printstrnew:                  ;显示指定的字符串, 以'$'为结束标记
      mov al,[si]
      cmp al,'$'
      je disovernew
      mov ah,0eh
      int 10h
      inc si
      jmp printstrnew
disovernew:
      ret

newlinenew:                     ;显示回车换行
      mov ah,0eh
      mov al,0dh
      int 10h
      mov al,0ah
      int 10h
      ret

;-----------------将16进制数字(1位)转换成ASCII码，输入DL,输出DL------
ASCIInew:   CMP  DL,9
         JG   LETTERnew  ;DL>OAH
         ADD  DL,30H  ;如果是数字,加30H即转换成ASCII码
         RET
LETTERnew:  ADD  DL,37H  ;如果是A～F,加37H即转换成ASCII码
         RET

;-----------------取出1个字节Byte的高4位和低4位,输入DL,输出BH和BL----------

HL4BITnew:  MOV  DH,dl
         MOV  BL,dl
         SHR  DH,1
         SHR  DH,1
         SHR  DH,1
         SHR  DH,1
         MOV  BH,DH                    ;取高4位
         AND  BL,0FH                   ;取低4位
         RET


next:
in al,0x92    ;打开A20地址线
or al,0000_0010B
out 0x92,al

cli ;保护模式下中断机制尚未建立，应禁止中断

mov eax,cr0  ;打开保护模式开关
or eax,1
mov cr0,eax

;进入保护模式... ...
jmp dword 0x0008:inprotectmode-512 ;16位的描述符选择子：32位偏移;这里需要扣除掉512B的MBR偏移量

[bits 32]
inprotectmode:

;在屏幕上显示"Protect mode",验证保护模式下的数据段设置正确
mov ax,00000000000_10_000B ;加载数据段选择子(0x10)
mov ds,ax

mov byte [0xb8000+20*160+0x00],'P'  ;屏幕第20行开始显示
mov byte [0xb8000+20*160+0x01],0x0c
mov byte [0xb8000+20*160+0x02],'R'
mov byte [0xb8000+20*160+0x03],0x0c
mov byte [0xb8000+20*160+0x04],'O'
mov byte [0xb8000+20*160+0x05],0x0c
mov byte [0xb8000+20*160+0x06],'T'
mov byte [0xb8000+20*160+0x07],0x0c
mov byte [0xb8000+20*160+0x08],'E'
mov byte [0xb8000+20*160+0x09],0x0c
mov byte [0xb8000+20*160+0x0a],'C'
mov byte [0xb8000+20*160+0x0b],0x0c
mov byte [0xb8000+20*160+0x0c],'T'
mov byte [0xb8000+20*160+0x0d],0x0c
mov byte [0xb8000+20*160+0x0e],'-'
mov byte [0xb8000+20*160+0x0f],0x0c
mov byte [0xb8000+20*160+0x10],'M'
mov byte [0xb8000+20*160+0x11],0x0c
mov byte [0xb8000+20*160+0x12],'O'
mov byte [0xb8000+20*160+0x13],0x0c
mov byte [0xb8000+20*160+0x14],'D'
mov byte [0xb8000+20*160+0x15],0x0c
mov byte [0xb8000+20*160+0x16],'E'
mov byte [0xb8000+20*160+0x17],0x0c
mov byte [0xb8000+20*160+0x18],' '
mov byte [0xb8000+20*160+0x19],0x0c
mov byte [0xb8000+20*160+0x1a],'!'
mov byte [0xb8000+20*160+0x1b],0x0c
mov byte [0xb8000+20*160+0x1c],'!'
mov byte [0xb8000+20*160+0x1d],0x0c
mov byte [0xb8000+20*160+0x1e],'!'
mov byte [0xb8000+20*160+0x1f],0x0c

;通过堆栈操作,验证保护模式下的堆栈段设置正确
mov ax,00000000000_11_000B ;加载堆栈段选择子
mov ss,ax                  ;7a00-7c00为此次设计的堆栈区
mov esp,0x7c00             ;7c00固定地址为栈底，
                           ;7a00为栈顶的最低地址（通过载堆栈段选择子的段界限值设置）
mov  ebp,esp ;保存堆栈指针
push byte '#' ;压入立即数#（字节）后，执行push指令，esp会自动减4

sub ebp,4

cmp ebp,esp ;判断ESP是否减4
jnz over    ;如果堆栈工作正常则打印出pop出来的值和其它字符

pop eax

mov byte [0xb8000+22*160+0x00],'S'
mov byte [0xb8000+22*160+0x01],0x0c
mov byte [0xb8000+22*160+0x02],'t'
mov byte [0xb8000+22*160+0x03],0x0c
mov byte [0xb8000+22*160+0x04],'a'
mov byte [0xb8000+22*160+0x05],0x0c
mov byte [0xb8000+22*160+0x06],'c'
mov byte [0xb8000+22*160+0x07],0x0c
mov byte [0xb8000+22*160+0x08],'k'
mov byte [0xb8000+22*160+0x09],0x0c
mov byte [0xb8000+22*160+0x0a],':'
mov byte [0xb8000+22*160+0x0b],0x0c
mov byte [0xb8000+22*160+0x0c],al     ;打印出pop出来的值
mov byte [0xb8000+22*160+0x0d],0x0c
mov byte [0xb8000+22*160+0x0e],','
mov byte [0xb8000+22*160+0x0f],0x0c
mov byte [0xb8000+22*160+0x10],'O'
mov byte [0xb8000+22*160+0x11],0x0c
mov byte [0xb8000+22*160+0x12],'K'
mov byte [0xb8000+22*160+0x13],0x0c
mov byte [0xb8000+22*160+0x14],'!'
mov byte [0xb8000+22*160+0x15],0x0c

over :
jmp $


3. 保护模式程序解析

在上面的演示程序中，我们把涉及保护模式的部分拿出来单独分析教学：

gdt_size dw 32-1     ;GDT 表的大小 ;（总字节数减一）
gdt_base dd 0x00007e00 ;GDT的物理地址

lgdt [gdt_size] ;将DPT的地址和大小写入gdtr生效     默认DS

;GDT创建
;创建0#描述符，它是空描述符，这是处理器的要求
mov dword [es:0x00],0x00
mov dword [es:0x04],0x00

;创建#1描述符，保护模式下的代码段描述符
mov dword [es:0x08],0x8000ffff
mov dword [es:0x0c],0x00409800

;创建#2描述符，保护模式下的数据段描述符（文本模式下的显示缓冲区）
mov dword [es:0x10],0x0000ffff  ;（把DS的基地址定义为0）
mov dword [es:0x14],0x00c09200  ; (标志位G=1,表示以4KB为单位)

;创建#3描述符，保护模式下的堆栈段描述符
mov dword [es:0x18],0x00007a00
mov dword [es:0x1c],0x00409600

in al,0x92    ;打开A20地址线
or al,0000_0010B
out 0x92,al

cli ;保护模式下中断机制尚未建立，应禁止中断

mov eax,cr0  ;打开保护模式开关
or eax,1
mov cr0,eax

;进入保护模式... ...
jmp dword 0x0008:inprotectmode ;16位的描述符选择子：32位偏移

[bits 32]
inprotectmode:

;在屏幕上显示"Protect mode",验证保护模式下的数据段设置正确
mov ax,00000000000_10_000B ;加载数据段选择子(0x10)
mov ds,ax

mov byte [20*160+0x00],'P'  ;屏幕第20行开始显示

;通过堆栈操作,验证保护模式下的堆栈段设置正确
mov ax,00000000000_11_000B ;加载堆栈段选择子
mov ss,ax                  ;7a00-7c00为此次设计的堆栈区
mov esp,0x7c00             ;7c00固定地址为栈底，
                           ;7a00为栈顶的最低地址（通过载堆栈段选择子的段界限值设置）
mov  ebp,esp ;保存堆栈指针
push byte '#' ;压入立即数#（字节）后，执行push指令，esp会自动减4

sub ebp,4

cmp ebp,esp ;判断ESP是否减4
jnz over    ;如果堆栈工作正常则打印出pop出来的值和其它字符

pop eax
mov byte [22*160+0x00],'S'
mov byte [22*160+0x01],0x0c

over  :
jmp $
