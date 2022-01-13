;nnos_app_hello（调用系统API打印hello world）
;[INSTRSET "i486p"]
[BITS 32]
	MOV ECX,msg				;将msg首地址赋值给ECX
	MOV EDX,1				;功能号为1，打印一行字符串
printloop:					;循环打印程序段
	MOV AL,[CS:ECX]			;将第一个字符送入AL
	CMP	AL,0				;判断字符串是否结束
	JE	return					;结束则跳转到fin，结束运行返回中断调用者
	INT 0x40				;若字符串未结束，则调用API中断，打印出字符
	ADD ECX,1				;将ECX偏移1
	JMP printloop			;跳转到printloop继续打印下一个字符
; fin:						;结束运行程序段
	; RETF					;返回中断调用者
return:						;结束运行程序段
	MOV EDX,4				;返回中断调用者
	INT 0x40				;调用API中断返回
msg:						;字符信息程序段
	;DB 0x0a					;换行
	DB "Hello world, nnos!",0 ;写入字符串
	
	;CLI ;屏蔽中断
;fin:
	;HLT ;CPU停机
	;JMP fin ;循环