jmp    newprogram


gdt_size dw 32-1     ;GDT ��Ĵ�С ;�����ֽ�����һ��
gdt_base dd 0x00007e00 ;GDT�������ַ

msgstep2:       db 'Step2:now  jmp  out  mbr','$'
mesmem2:        db 'Will Visit Memory Address is---','$'
msgcs2:         db '0X:????(CS):XXXX','$'

msgstep3:       db 'Step3:now  enter  protect  mode','$'

newprogram:
mov     ax,newseg      ;��ת���µ�ַ8000H֮��ȫ���Ĵ��������µĶε�ַ
sub     ax,20h         ;Ҫ����һ��DS��ֵ������ȷ�����³����е�����
mov     ds,ax          ;ds���ڴ�ӡ���ݵ�Ѱַ

call  outmbr
call  showcsnew

call  showprotect



mov     ax,dptseg
mov     es,ax          ;es����gpt��Ѱַ    gpt�����ʼ��ַ:0x00007e00h
call    createdpt

jmp  next





;����DPT�ӳ���
createdpt:

lgdt [gdt_size] ;��DPT�ĵ�ַ�ʹ�Сд��gdtr��Ч     Ĭ��DS

;����0#�����������ǿ������������Ǵ�������Ҫ��
mov dword [es:0x00],0x00
mov dword [es:0x04],0x00

;����#1������������ģʽ�µĴ����������
mov dword [es:0x08],0x8000ffff
mov dword [es:0x0c],0x00409800

;����#2������������ģʽ�µ����ݶ����������ı�ģʽ�µ���ʾ��������
;mov dword [es:0x10],0x8000ffff
;mov dword [es:0x14],0x0040920b

mov dword [es:0x10],0x0000ffff  ;����DS�Ļ���ַ����Ϊ0��
mov dword [es:0x14],0x00c09200  ; (��־λG=1,��ʾ��KBΪ��λ)

;����#3������������ģʽ�µĶ�ջ��������
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


showcsnew:                       ;չʾCS��ֵ
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

printstrnew:                  ;��ʾָ�����ַ���, ��'$'Ϊ�������
      mov al,[si]
      cmp al,'$'
      je disovernew
      mov ah,0eh
      int 10h
      inc si
      jmp printstrnew
disovernew:
      ret

newlinenew:                     ;��ʾ�س�����
      mov ah,0eh
      mov al,0dh
      int 10h
      mov al,0ah
      int 10h
      ret

;-----------------��16��������(1λ)ת����ASCII�룬����DL,���DL------
ASCIInew:   CMP  DL,9
         JG   LETTERnew  ;DL>OAH
         ADD  DL,30H  ;���������,��30H��ת����ASCII��
         RET
LETTERnew:  ADD  DL,37H  ;�����A��F,��37H��ת����ASCII��
         RET

;-----------------ȡ��1���ֽ�Byte�ĸ�4λ�͵�4λ,����DL,���BH��BL----------

HL4BITnew:  MOV  DH,dl
         MOV  BL,dl
         SHR  DH,1
         SHR  DH,1
         SHR  DH,1
         SHR  DH,1
         MOV  BH,DH                    ;ȡ��4λ
         AND  BL,0FH                   ;ȡ��4λ
         RET


next:
in al,0x92    ;��A20��ַ��
or al,0000_0010B
out 0x92,al

cli ;����ģʽ���жϻ�����δ������Ӧ��ֹ�ж�

mov eax,cr0  ;�򿪱���ģʽ����
or eax,1
mov cr0,eax

;���뱣��ģʽ... ...
jmp dword 0x0008:inprotectmode-512 ;16λ��������ѡ���ӣ�32λƫ��;������Ҫ�۳���512B��MBRƫ����

[bits 32]
inprotectmode:

;����Ļ����ʾ"Protect mode",��֤����ģʽ�µ����ݶ�������ȷ
mov ax,00000000000_10_000B ;�������ݶ�ѡ����(0x10)
mov ds,ax

mov byte [0xb8000+20*160+0x00],'P'  ;��Ļ��20�п�ʼ��ʾ
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

;ͨ����ջ����,��֤����ģʽ�µĶ�ջ��������ȷ
mov ax,00000000000_11_000B ;���ض�ջ��ѡ����
mov ss,ax                  ;7a00-7c00Ϊ�˴���ƵĶ�ջ��
mov esp,0x7c00             ;7c00�̶���ַΪջ�ף�
                           ;7a00Ϊջ������͵�ַ��ͨ���ض�ջ��ѡ���ӵĶν���ֵ���ã�
mov  ebp,esp ;�����ջָ��
push byte '#' ;ѹ��������#���ֽڣ���ִ��pushָ�esp���Զ���4

sub ebp,4

cmp ebp,esp ;�ж�ESP�Ƿ��4
jnz over    ;�����ջ�����������ӡ��pop������ֵ�������ַ�

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
mov byte [0xb8000+22*160+0x0c],al     ;��ӡ��pop������ֵ
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


3. ����ģʽ�������

���������ʾ�����У����ǰ��漰����ģʽ�Ĳ����ó�������������ѧ��

gdt_size dw 32-1     ;GDT ��Ĵ�С ;�����ֽ�����һ��
gdt_base dd 0x00007e00 ;GDT�������ַ

lgdt [gdt_size] ;��DPT�ĵ�ַ�ʹ�Сд��gdtr��Ч     Ĭ��DS

;GDT����
;����0#�����������ǿ������������Ǵ�������Ҫ��
mov dword [es:0x00],0x00
mov dword [es:0x04],0x00

;����#1������������ģʽ�µĴ����������
mov dword [es:0x08],0x8000ffff
mov dword [es:0x0c],0x00409800

;����#2������������ģʽ�µ����ݶ����������ı�ģʽ�µ���ʾ��������
mov dword [es:0x10],0x0000ffff  ;����DS�Ļ���ַ����Ϊ0��
mov dword [es:0x14],0x00c09200  ; (��־λG=1,��ʾ��4KBΪ��λ)

;����#3������������ģʽ�µĶ�ջ��������
mov dword [es:0x18],0x00007a00
mov dword [es:0x1c],0x00409600

in al,0x92    ;��A20��ַ��
or al,0000_0010B
out 0x92,al

cli ;����ģʽ���жϻ�����δ������Ӧ��ֹ�ж�

mov eax,cr0  ;�򿪱���ģʽ����
or eax,1
mov cr0,eax

;���뱣��ģʽ... ...
jmp dword 0x0008:inprotectmode ;16λ��������ѡ���ӣ�32λƫ��

[bits 32]
inprotectmode:

;����Ļ����ʾ"Protect mode",��֤����ģʽ�µ����ݶ�������ȷ
mov ax,00000000000_10_000B ;�������ݶ�ѡ����(0x10)
mov ds,ax

mov byte [20*160+0x00],'P'  ;��Ļ��20�п�ʼ��ʾ

;ͨ����ջ����,��֤����ģʽ�µĶ�ջ��������ȷ
mov ax,00000000000_11_000B ;���ض�ջ��ѡ����
mov ss,ax                  ;7a00-7c00Ϊ�˴���ƵĶ�ջ��
mov esp,0x7c00             ;7c00�̶���ַΪջ�ף�
                           ;7a00Ϊջ������͵�ַ��ͨ���ض�ջ��ѡ���ӵĶν���ֵ���ã�
mov  ebp,esp ;�����ջָ��
push byte '#' ;ѹ��������#���ֽڣ���ִ��pushָ�esp���Զ���4

sub ebp,4

cmp ebp,esp ;�ж�ESP�Ƿ��4
jnz over    ;�����ջ�����������ӡ��pop������ֵ�������ַ�

pop eax
mov byte [22*160+0x00],'S'
mov byte [22*160+0x01],0x0c

over  :
jmp $
