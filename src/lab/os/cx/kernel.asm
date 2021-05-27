xnumadd    equ   0x7d00   ;分辨率x存放地址 
colorfuncport equ 3c8h   ;设置调色板功能端口
colorsetport equ 3c9h    ;设置调色板颜色端口
displayadd equ 0xa000    ;低分辨率640*480下显示缓冲区地址 
displayaddhigh equ 0x9000    ;存放高分辨率640*480下"显存线性缓冲区地址"的地址 
dptseg         equ    7e0h     ;DPT区段地址
VESA       equ  0
 
 
jmp   start  
 
gdt_size dw 80-1     ;GDT 表的大小 ;（总字节数减一,共5个描述符）
gdt_base dd 0x00007e00 ;GDT的物理地址
 
showmsg1     db '      Please Select Windows Resolution:$'
showmsg2     db '      1:1024X768$'
showmsg3     db '      2:640X480$'
inputcom     db '?'
noword       db '      :$'
megerror     db '      Input Error,Please Retry$'
 
 
start:    
mov  ax, cs     ;从MBR跳转到此内核之后,CS=c20h ,IP=0。也即程序从偏移地址为0的地方开始放置    
mov  ds, ax     ;那么就无需指定ORG，只需要把DS,ES和CS指向同一段即可。
 
mov  ax, xnumadd/10h  ;用于存放分辨率数据 
mov  es, ax
 
call   showmsg
call   userinput
 
call  setmode
call  backgroud
call  colorset
 
call  getdisplayaddr
 
mov     ax,dptseg
mov     es,ax          ;es用于gpt区寻址    gpt存放起始地址:0x00007e00h
call    createdpt   
jmp     protectmode
 
showmsg:
mov    cx,25
clear:
call   newline
loop   clear
 
mov    si, showmsg1
call   printstr
call   newline
call   newline
mov    si, showmsg2
call   printstr
call   newline
call   newline
mov    si, showmsg3
call   printstr
call   newline
call   newline
 
mov    cx,10
clear2:
call   newline
loop   clear2
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
 
          
userinput: mov     si, 0
input: 
           mov ah,0
           int 16h                        ;从键盘读字符 ah=扫描码 al=字符码
           mov ah,0eh                     ;把键盘输入的字符显示出来 
           int 10h
           cmp    al, 0dh                 ;回车作为输入结束标记
           je     inputover
           mov    [inputcom+si],al
           inc    si
           jmp    input
 
inputover: mov  bl, [inputcom]
           cmp  bl,'1'
           jne  inputislow
           mov word [es:0],1024
           mov word [es:2],768
           jmp totalover
inputislow:cmp  bl, '2'
           jne inputerror
           mov word [es:0],640
           mov word [es:2],480
           jmp totalover      
  inputerror: mov si, megerror
              call printstr  
              call newline
              mov  si,noword
              call printstr 
              jmp  userinput
   totalover: ret  
 
 
setmode:   
mov AX,4F02H
mov dx,word [es:0] 
cmp dx,1024
jne lowmode
mov bx,4105H   
jmp setover     ;1024*768 256色   第1个值4表示为要使用线性地址模式
lowmode:
mov bx,4101H        ;640*480  256色   第1个值4表示为要使用线性地址模式
setover: 
int 10h
ret
 
 
;取得该模式下显存线性地址,存入:90040处。调用0x4F01 功能号，获得信息
getdisplayaddr:
mov ax,displayaddhigh
mov es,ax
mov di,  0
 
mov ax , 0x4f01
mov cx , 0x101
int 0x10
;返回的结构体中偏移量为40 的地方，即es:di+40 处，用4字节存放了一个线性地址，
;这就是这个显卡在此模式下显存的线性地址
;dw  0x1234 --- 0x34 0x12  注意DWORD的内存放置顺序，低地址为低位，高地址为高位 
;[ es:VESA+40 ]低位,[ es:VESA+42 ] 高位，所以显存起始地址是: 0xe0000000
mov ebx,  [ es:VESA+40 ]   ;显卡线性地址送入ebx，后面创建GDT使用 
ret
   
 
;通过普通的0xa000方式演示画图 
drawimg:           ;满屏画同一颜色 
mov bl,3
mov ax,displayadd
mov es,ax
mov cx,0xffff
mov di,0
nextpoint:
mov  [es:di],bl   ;调色板颜色索引送往显存地址
inc di
loop  nextpoint
ret
 
 
backgroud:         ;背景色设置
mov dx,  colorfuncport
mov al,  0           ;建调色板索引0号
out dx,al
 
mov dx,  colorsetport   ;设置蓝色背景
mov al,0           ;R分量
out dx,al
mov al,0           ;G分量
out dx,al
mov al,35          ;B分量
out dx,al
ret
 
colorset:             ;显示色设置
mov dx,  colorfuncport
mov al,  1              ;建调色板索引1号
out dx,al
 
mov dx,  colorsetport     ;设置白色调色板
mov al,63           ;R分量
out dx,al
mov al,63           ;G分量
out dx,al
mov al,63          ;B分量
out dx,al
 
mov dx,  colorfuncport
mov al,  2                 ;建调色板索引2号
out dx,al
 
mov dx,  colorsetport     ;设置红色调色板
mov al,63           ;R分量
out dx,al
mov al,0           ;G分量
out dx,al
mov al,0          ;B分量
out dx,al
 
mov dx,  colorfuncport
mov al, 3           ;建调色板索引3号
out dx,al
 
mov dx,  colorsetport     ;设置黄色
mov al,30           ;R分量
out dx,al
mov al,30           ;G分量
out dx,al
mov al,0          ;B分量
out dx,al
 
mov dx,  colorfuncport
mov al, 4              ;建调色板索引4号
out dx,al
 
mov dx,  colorsetport     ;设置黑色
mov al,0           ;R分量
out dx,al
mov al,0           ;G分量
out dx,al
mov al,0          ;B分量
out dx,al
 
ret
 
 
;创建DPT子程序
createdpt:
 
lgdt [gdt_size] ;将DPT的地址和大小写入gdtr生效     默认DS
 
 
;创建0#描述符，它是空描述符，这是处理器的要求
mov dword [es:0x00],0x00             ;00
mov dword [es:0x04],0x00
 
;创建#1描述符，保护模式下的代码段描述符:CS
mov dword [es:0x08],0xc200ffff       ;01
mov dword [es:0x0c],0x00409800  ;0x0000_c200
 
;创建#2描述符，保护模式下的数据段描述符：DS（文本模式下的显示缓冲区） 10
mov dword [es:0x10],0x8000ffff
mov dword [es:0x14],0x0040920b  ;0x000b_8000    
 
;创建#3描述符，保护模式下的数据段2描述符:ES （图形模式下的线性地址显示缓冲区）11
 
;mov dword [es:0x18],0x0000ffff  
;mov dword [es:0x1c],0xe04f9200  ;0xe000_0000 
                           
mov word [es:0x18],0xffff  ;显卡线性地址的低16位
mov byte [es:0x1a],bl      ;0x00  ;由于象素较多，段界限设置成0xfffff
mov byte [es:0x1b],bh      ;0x00
 
shr ebx,16  ;取出ebx的高16位,显卡线性地址的高16位 
mov byte [es:0x1c],bl          ;0x00     
mov word [es:0x1d],0x4f92      ;f位段界限最高位
mov byte [es:0x1f],bh          ;0xe0    
 
 
;创建#4描述符，保护模式下的数据段描述符：DS（用户设置的分辨率） 100
mov dword [es:0x20],0x7d000100
mov dword [es:0x24],0x00409200  ;0x0000_7d00
 
;创建#5描述符，保护模式下的堆栈段描述符:SS
mov dword [es:0x28],0x00007a00  ;0x0000_7a00
mov dword [es:0x2c],0x00409600
 
ret
 
 
protectmode:
in al,0x92    ;打开A20地址线
or al,0000_0010B
out 0x92,al
 
cli ;保护模式下中断机制尚未建立，应禁止中断
 
mov eax,cr0  ;打开保护模式开关
or eax,1
mov cr0,eax 
 
;进入保护模式... ...
 
jmp dword 0x0008:inprotectmode ;16位的描述符选择子：
;32位偏移;这里需要扣除掉512B的MBR偏移量
 
[bits 32]
inprotectmode:
 
mov ax,00000000000_11_000B ;加载数据段选择子(0x11)
mov es,ax
 
call getmode
jmp  $
 
 
getmode:
mov ax,00000000001_00_000B ;加载数据段选择子(0x11)
mov ds,ax
mov bp,[00]   ;x     640
mov si,[02]   ;y     480
;mov bp,1024
cmp bp,1024
jne low
call win11024
call win21024
call win31024
call win41024
jmp  modeover
low:
call win1640
call win2640
call win3640
call win4640
modeover:
ret
 
 
;通过线性显存地址的方式绘制矩形 
 
win1640:  
mov edx,0             ;起始行 y坐标 
win1line:   
 
imul ebx,edx,640   ;edx*100放入ebx
 
mov edi,0           ;起始列 x 坐标
mov eax,edi         ;dx+edi
add eax,ebx
mov edi,eax
mov ecx,0
win1point:
mov  byte [es:edi],1   ;调色板颜色索引送往显存地址 
inc  edi
inc ecx
cmp ecx,640/2-640/30      ;矩形长度
jne win1point
 
inc edx
 
cmp edx,480/2-480/30       ;矩形高度
jne win1line
ret  
 
 
win2640:
mov edx,0             ;起始行 y坐标
win2line:
 
imul ebx,edx,640   ;edx*100放入ebx
 
mov edi,640/2           ;起始列 x 坐标
mov eax,edi         ;dx+edi
add eax,ebx
mov edi,eax
mov ecx,0
win2point:
mov  byte [es:edi],2   ;调色板颜色索引送往显存地址
inc  edi
inc ecx
cmp ecx,640/2      ;矩形长度
jne win2point
 
inc edx
 
cmp edx,480/2       ;矩形高度
jne win2line
ret
 
 
win3640:
mov edx,480/2            ;起始行 y坐标
win3line:
 
imul ebx,edx,640   ;edx*100放入ebx
 
mov edi,0           ;起始列 x 坐标
mov eax,edi         ;dx+edi
add eax,ebx
mov edi,eax
mov ecx,0
win3point:
mov  byte [es:edi],3   ;调色板颜色索引送往显存地址
inc  edi
inc ecx
cmp ecx,640/2      ;矩形长度
jne win3point
 
inc edx
 
cmp edx,640       ;矩形高度
jne win3line
ret
 
win4640:
mov edx,480/2+480/30            ;起始行 y坐标
win4line:
 
imul ebx,edx,640   ;edx*100放入ebx
 
mov edi,640/2+640/30           ;起始列 x 坐标
mov eax,edi         ;dx+edi
add eax,ebx
mov edi,eax
mov ecx,0
win4point:
mov  byte [es:edi],4   ;调色板颜色索引送往显存地址
inc  edi
inc ecx
cmp ecx,640/2-640/30      ;矩形长度
jne win4point
 
inc edx
 
cmp edx,480       ;矩形高度
jne win4line
ret
 
 
win11024:
mov edx,0             ;起始行 y坐标
win1line1024:
 
imul ebx,edx,1024   ;edx*100放入ebx
 
mov edi,0           ;起始列 x 坐标
mov eax,edi         ;dx+edi
add eax,ebx
mov edi,eax
mov ecx,0
win1point1024:
mov  byte [es:edi],1   ;调色板颜色索引送往显存地址
inc  edi
inc ecx
cmp ecx,1024/2-1024/30      ;矩形长度
jne win1point1024
 
inc edx
 
cmp edx,768/2-768/30       ;矩形高度
jne win1line1024
ret
 
 
win21024:
mov edx,0             ;起始行 y坐标
win2line1024:
 
imul ebx,edx,1024   ;edx*100放入ebx
 
mov edi,1024/2           ;起始列 x 坐标
mov eax,edi         ;dx+edi
add eax,ebx
mov edi,eax
mov ecx,0
win2point1024:
mov  byte [es:edi],2   ;调色板颜色索引送往显存地址
inc  edi
inc ecx
cmp ecx,1024/2      ;矩形长度
jne win2point1024
 
inc edx
 
cmp edx,768/2       ;矩形高度
jne win2line1024
ret
 
 
win31024:
mov edx,768/2            ;起始行 y坐标
win3line1024:
 
imul ebx,edx,1024   ;edx*100放入ebx
 
mov edi,0           ;起始列 x 坐标
mov eax,edi         ;dx+edi
add eax,ebx
mov edi,eax
mov ecx,0
win3point1024:
mov  byte [es:edi],3   ;调色板颜色索引送往显存地址
inc  edi
inc ecx
cmp ecx,1024/2      ;矩形长度
jne win3point1024
 
inc edx
 
cmp edx,768       ;矩形高度
jne win3line1024
ret
 
 
win41024:
mov edx,768/2+768/30            ;起始行 y坐标
win4line1024:
 
imul ebx,edx,1024   ;edx*100放入ebx
 
mov edi,1024/2+1024/30           ;起始列 x 坐标
mov eax,edi         ;dx+edi
add eax,ebx
mov edi,eax
mov ecx,0
win4point1024:
mov  byte [es:edi],4   ;调色板颜色索引送往显存地址
inc  edi
inc ecx
cmp ecx,1024/2-1024/30      ;矩形长度
jne win4point1024
 
inc edx
 
cmp edx,768       ;矩形高度
jne win4line1024
ret
tails/112590494