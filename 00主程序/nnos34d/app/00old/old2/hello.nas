;nnos_app_hlt（CPU停机等待应用程序）
[INSTRSET "i486p"]
[BITS 32]
	MOV ECX,msg				;将msg首地址赋值给ECX
printloop:					;循环打印程序段
	MOV AL,[CS:ECX]			;将第一个字符送入AL
	CMP	AL,0				;判断字符串是否结束
	JE	fin					;结束则跳转到fin，结束运行返回中断调用者
	INT 0x40				;若字符串未结束，则调用API中断，打印出字符
	ADD ECX,1				;将ECX偏移1
	JMP printloop			;跳转到printloop继续打印下一个字符
fin:						;结束运行程序段
	RETF					;返回中断调用者
msg:						;字符信息程序段
	DB "Hello world, nnos!",0 ;写入字符串
	
	;CLI ;屏蔽中断
;fin:
	;HLT ;CPU停机
	;JMP fin ;循环