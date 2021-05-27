xnumadd    equ   0x7d00   ;�ֱ���x��ŵ�ַ 
colorfuncport equ 3c8h   ;���õ�ɫ�幦�ܶ˿�
colorsetport equ 3c9h    ;���õ�ɫ����ɫ�˿�
displayadd equ 0xa000    ;�ͷֱ���640*480����ʾ��������ַ 
displayaddhigh equ 0x9000    ;��Ÿ߷ֱ���640*480��"�Դ����Ի�������ַ"�ĵ�ַ 
dptseg         equ    7e0h     ;DPT���ε�ַ
VESA       equ  0
 
 
jmp   start  
 
gdt_size dw 80-1     ;GDT ��Ĵ�С ;�����ֽ�����һ,��5����������
gdt_base dd 0x00007e00 ;GDT�������ַ
 
showmsg1     db '      Please Select Windows Resolution:$'
showmsg2     db '      1:1024X768$'
showmsg3     db '      2:640X480$'
inputcom     db '?'
noword       db '      :$'
megerror     db '      Input Error,Please Retry$'
 
 
start:    
mov  ax, cs     ;��MBR��ת�����ں�֮��,CS=c20h ,IP=0��Ҳ�������ƫ�Ƶ�ַΪ0�ĵط���ʼ����    
mov  ds, ax     ;��ô������ָ��ORG��ֻ��Ҫ��DS,ES��CSָ��ͬһ�μ��ɡ�
 
mov  ax, xnumadd/10h  ;���ڴ�ŷֱ������� 
mov  es, ax
 
call   showmsg
call   userinput
 
call  setmode
call  backgroud
call  colorset
 
call  getdisplayaddr
 
mov     ax,dptseg
mov     es,ax          ;es����gpt��Ѱַ    gpt�����ʼ��ַ:0x00007e00h
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
 
printstr:                  ;��ʾָ�����ַ���, ��'$'Ϊ�������
      mov al,[si]
      cmp al,'$'
      je disover
      mov ah,0eh
      int 10h
      inc si
      jmp printstr
disover:
      ret
 
newline:                     ;��ʾ�س�����
      mov ah,0eh
      mov al,0dh
      int 10h
      mov al,0ah
      int 10h
      ret
 
          
userinput: mov     si, 0
input: 
           mov ah,0
           int 16h                        ;�Ӽ��̶��ַ� ah=ɨ���� al=�ַ���
           mov ah,0eh                     ;�Ѽ���������ַ���ʾ���� 
           int 10h
           cmp    al, 0dh                 ;�س���Ϊ����������
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
jmp setover     ;1024*768 256ɫ   ��1��ֵ4��ʾΪҪʹ�����Ե�ַģʽ
lowmode:
mov bx,4101H        ;640*480  256ɫ   ��1��ֵ4��ʾΪҪʹ�����Ե�ַģʽ
setover: 
int 10h
ret
 
 
;ȡ�ø�ģʽ���Դ����Ե�ַ,����:90040��������0x4F01 ���ܺţ������Ϣ
getdisplayaddr:
mov ax,displayaddhigh
mov es,ax
mov di,  0
 
mov ax , 0x4f01
mov cx , 0x101
int 0x10
;���صĽṹ����ƫ����Ϊ40 �ĵط�����es:di+40 ������4�ֽڴ����һ�����Ե�ַ��
;���������Կ��ڴ�ģʽ���Դ�����Ե�ַ
;dw  0x1234 --- 0x34 0x12  ע��DWORD���ڴ����˳�򣬵͵�ַΪ��λ���ߵ�ַΪ��λ 
;[ es:VESA+40 ]��λ,[ es:VESA+42 ] ��λ�������Դ���ʼ��ַ��: 0xe0000000
mov ebx,  [ es:VESA+40 ]   ;�Կ����Ե�ַ����ebx�����洴��GDTʹ�� 
ret
   
 
;ͨ����ͨ��0xa000��ʽ��ʾ��ͼ 
drawimg:           ;������ͬһ��ɫ 
mov bl,3
mov ax,displayadd
mov es,ax
mov cx,0xffff
mov di,0
nextpoint:
mov  [es:di],bl   ;��ɫ����ɫ���������Դ��ַ
inc di
loop  nextpoint
ret
 
 
backgroud:         ;����ɫ����
mov dx,  colorfuncport
mov al,  0           ;����ɫ������0��
out dx,al
 
mov dx,  colorsetport   ;������ɫ����
mov al,0           ;R����
out dx,al
mov al,0           ;G����
out dx,al
mov al,35          ;B����
out dx,al
ret
 
colorset:             ;��ʾɫ����
mov dx,  colorfuncport
mov al,  1              ;����ɫ������1��
out dx,al
 
mov dx,  colorsetport     ;���ð�ɫ��ɫ��
mov al,63           ;R����
out dx,al
mov al,63           ;G����
out dx,al
mov al,63          ;B����
out dx,al
 
mov dx,  colorfuncport
mov al,  2                 ;����ɫ������2��
out dx,al
 
mov dx,  colorsetport     ;���ú�ɫ��ɫ��
mov al,63           ;R����
out dx,al
mov al,0           ;G����
out dx,al
mov al,0          ;B����
out dx,al
 
mov dx,  colorfuncport
mov al, 3           ;����ɫ������3��
out dx,al
 
mov dx,  colorsetport     ;���û�ɫ
mov al,30           ;R����
out dx,al
mov al,30           ;G����
out dx,al
mov al,0          ;B����
out dx,al
 
mov dx,  colorfuncport
mov al, 4              ;����ɫ������4��
out dx,al
 
mov dx,  colorsetport     ;���ú�ɫ
mov al,0           ;R����
out dx,al
mov al,0           ;G����
out dx,al
mov al,0          ;B����
out dx,al
 
ret
 
 
;����DPT�ӳ���
createdpt:
 
lgdt [gdt_size] ;��DPT�ĵ�ַ�ʹ�Сд��gdtr��Ч     Ĭ��DS
 
 
;����0#�����������ǿ������������Ǵ�������Ҫ��
mov dword [es:0x00],0x00             ;00
mov dword [es:0x04],0x00
 
;����#1������������ģʽ�µĴ����������:CS
mov dword [es:0x08],0xc200ffff       ;01
mov dword [es:0x0c],0x00409800  ;0x0000_c200
 
;����#2������������ģʽ�µ����ݶ���������DS���ı�ģʽ�µ���ʾ�������� 10
mov dword [es:0x10],0x8000ffff
mov dword [es:0x14],0x0040920b  ;0x000b_8000    
 
;����#3������������ģʽ�µ����ݶ�2������:ES ��ͼ��ģʽ�µ����Ե�ַ��ʾ��������11
 
;mov dword [es:0x18],0x0000ffff  
;mov dword [es:0x1c],0xe04f9200  ;0xe000_0000 
                           
mov word [es:0x18],0xffff  ;�Կ����Ե�ַ�ĵ�16λ
mov byte [es:0x1a],bl      ;0x00  ;�������ؽ϶࣬�ν������ó�0xfffff
mov byte [es:0x1b],bh      ;0x00
 
shr ebx,16  ;ȡ��ebx�ĸ�16λ,�Կ����Ե�ַ�ĸ�16λ 
mov byte [es:0x1c],bl          ;0x00     
mov word [es:0x1d],0x4f92      ;fλ�ν������λ
mov byte [es:0x1f],bh          ;0xe0    
 
 
;����#4������������ģʽ�µ����ݶ���������DS���û����õķֱ��ʣ� 100
mov dword [es:0x20],0x7d000100
mov dword [es:0x24],0x00409200  ;0x0000_7d00
 
;����#5������������ģʽ�µĶ�ջ��������:SS
mov dword [es:0x28],0x00007a00  ;0x0000_7a00
mov dword [es:0x2c],0x00409600
 
ret
 
 
protectmode:
in al,0x92    ;��A20��ַ��
or al,0000_0010B
out 0x92,al
 
cli ;����ģʽ���жϻ�����δ������Ӧ��ֹ�ж�
 
mov eax,cr0  ;�򿪱���ģʽ����
or eax,1
mov cr0,eax 
 
;���뱣��ģʽ... ...
 
jmp dword 0x0008:inprotectmode ;16λ��������ѡ���ӣ�
;32λƫ��;������Ҫ�۳���512B��MBRƫ����
 
[bits 32]
inprotectmode:
 
mov ax,00000000000_11_000B ;�������ݶ�ѡ����(0x11)
mov es,ax
 
call getmode
jmp  $
 
 
getmode:
mov ax,00000000001_00_000B ;�������ݶ�ѡ����(0x11)
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
 
 
;ͨ�������Դ��ַ�ķ�ʽ���ƾ��� 
 
win1640:  
mov edx,0             ;��ʼ�� y���� 
win1line:   
 
imul ebx,edx,640   ;edx*100����ebx
 
mov edi,0           ;��ʼ�� x ����
mov eax,edi         ;dx+edi
add eax,ebx
mov edi,eax
mov ecx,0
win1point:
mov  byte [es:edi],1   ;��ɫ����ɫ���������Դ��ַ 
inc  edi
inc ecx
cmp ecx,640/2-640/30      ;���γ���
jne win1point
 
inc edx
 
cmp edx,480/2-480/30       ;���θ߶�
jne win1line
ret  
 
 
win2640:
mov edx,0             ;��ʼ�� y����
win2line:
 
imul ebx,edx,640   ;edx*100����ebx
 
mov edi,640/2           ;��ʼ�� x ����
mov eax,edi         ;dx+edi
add eax,ebx
mov edi,eax
mov ecx,0
win2point:
mov  byte [es:edi],2   ;��ɫ����ɫ���������Դ��ַ
inc  edi
inc ecx
cmp ecx,640/2      ;���γ���
jne win2point
 
inc edx
 
cmp edx,480/2       ;���θ߶�
jne win2line
ret
 
 
win3640:
mov edx,480/2            ;��ʼ�� y����
win3line:
 
imul ebx,edx,640   ;edx*100����ebx
 
mov edi,0           ;��ʼ�� x ����
mov eax,edi         ;dx+edi
add eax,ebx
mov edi,eax
mov ecx,0
win3point:
mov  byte [es:edi],3   ;��ɫ����ɫ���������Դ��ַ
inc  edi
inc ecx
cmp ecx,640/2      ;���γ���
jne win3point
 
inc edx
 
cmp edx,640       ;���θ߶�
jne win3line
ret
 
win4640:
mov edx,480/2+480/30            ;��ʼ�� y����
win4line:
 
imul ebx,edx,640   ;edx*100����ebx
 
mov edi,640/2+640/30           ;��ʼ�� x ����
mov eax,edi         ;dx+edi
add eax,ebx
mov edi,eax
mov ecx,0
win4point:
mov  byte [es:edi],4   ;��ɫ����ɫ���������Դ��ַ
inc  edi
inc ecx
cmp ecx,640/2-640/30      ;���γ���
jne win4point
 
inc edx
 
cmp edx,480       ;���θ߶�
jne win4line
ret
 
 
win11024:
mov edx,0             ;��ʼ�� y����
win1line1024:
 
imul ebx,edx,1024   ;edx*100����ebx
 
mov edi,0           ;��ʼ�� x ����
mov eax,edi         ;dx+edi
add eax,ebx
mov edi,eax
mov ecx,0
win1point1024:
mov  byte [es:edi],1   ;��ɫ����ɫ���������Դ��ַ
inc  edi
inc ecx
cmp ecx,1024/2-1024/30      ;���γ���
jne win1point1024
 
inc edx
 
cmp edx,768/2-768/30       ;���θ߶�
jne win1line1024
ret
 
 
win21024:
mov edx,0             ;��ʼ�� y����
win2line1024:
 
imul ebx,edx,1024   ;edx*100����ebx
 
mov edi,1024/2           ;��ʼ�� x ����
mov eax,edi         ;dx+edi
add eax,ebx
mov edi,eax
mov ecx,0
win2point1024:
mov  byte [es:edi],2   ;��ɫ����ɫ���������Դ��ַ
inc  edi
inc ecx
cmp ecx,1024/2      ;���γ���
jne win2point1024
 
inc edx
 
cmp edx,768/2       ;���θ߶�
jne win2line1024
ret
 
 
win31024:
mov edx,768/2            ;��ʼ�� y����
win3line1024:
 
imul ebx,edx,1024   ;edx*100����ebx
 
mov edi,0           ;��ʼ�� x ����
mov eax,edi         ;dx+edi
add eax,ebx
mov edi,eax
mov ecx,0
win3point1024:
mov  byte [es:edi],3   ;��ɫ����ɫ���������Դ��ַ
inc  edi
inc ecx
cmp ecx,1024/2      ;���γ���
jne win3point1024
 
inc edx
 
cmp edx,768       ;���θ߶�
jne win3line1024
ret
 
 
win41024:
mov edx,768/2+768/30            ;��ʼ�� y����
win4line1024:
 
imul ebx,edx,1024   ;edx*100����ebx
 
mov edi,1024/2+1024/30           ;��ʼ�� x ����
mov eax,edi         ;dx+edi
add eax,ebx
mov edi,eax
mov ecx,0
win4point1024:
mov  byte [es:edi],4   ;��ɫ����ɫ���������Դ��ַ
inc  edi
inc ecx
cmp ecx,1024/2-1024/30      ;���γ���
jne win4point1024
 
inc edx
 
cmp edx,768       ;���θ߶�
jne win4line1024
ret
tails/112590494