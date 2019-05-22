;nnos_app_hlt（CPU停机等待应用程序）

[BITS 32]
	;CLI ;屏蔽中断
	MOV AL,'H' ;将字符‘A’存入AL寄存器,属于API
	;CALL 0x00003C0C ;属于API,应用程序与操作系统段号不同，前者1003*8,后者2*8，跳转需要加段号，用far-CALL,否则系统会崩溃
	;CALL 2*8:0x00001156
	INT 0x40
	MOV AL,'e'
	;CALL 2*8:0x00001156
	INT 0x40
	MOV AL,'l'
	INT 0x40
	MOV AL,'l'
	INT 0x40
	MOV AL,'o'
	INT 0x40
	MOV AL,' '
	INT 0x40
	MOV AL,'w'
	INT 0x40
	MOV AL,'o'
	INT 0x40
	MOV AL,'r'
	INT 0x40
	MOV AL,'l'
	INT 0x40
	MOV AL,'d'
	INT 0x40
	MOV AL,','
	INT 0x40
	MOV AL,' '
	INT 0x40	
	MOV AL,'n'
	INT 0x40	
	MOV AL,'n'
	INT 0x40	
	MOV AL,'o'
	INT 0x40
	MOV AL,'s'
	INT 0x40	
	MOV AL,'.'
	INT 0x40	
	RETF ;返回调用者
;fin:
	;HLT ;CPU停机
	;JMP fin ;循环