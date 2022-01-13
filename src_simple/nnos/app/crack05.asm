;HLT病毒
;应用程序调用系统函数只能通过API中断，不能CALL
;[INSTRSET "i486"]
[BITS 32]
	CALL 2*8:0x1024 	;调用中断函数
return:
	MOV EDX,4
	INT 0x40