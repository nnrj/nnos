;nnos_app_hello02（调用系统API打印hello world）
;[INSTRSET "i486p"]
[BITS 32]
	MOV EDX,2	 ;向edx中写入2，功能号，为打印一行字符串
	MOV EBX,msg	 ;将msg地址写入EBX
	INT 0x40	 ;产生API中断
	; RETF		 ;返回调用者
return:
	MOV EDX,4
	INT 0x40
	
msg:
	;DB 0x0a					;换行
	DB "Hello world agin, nnos!",0 ;写入字符串