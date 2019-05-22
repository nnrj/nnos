[FORMAT "WCOFF"]
[INSTRSET "i486p"]				
[BITS 32]						
[FILE "a_nask.nas"]				

	GLOBAL	_api_printc,_api_printl 						;字符输出API接口
	GLOBAL _api_return										;进程相关API接口
	GLOBAL _api_window,_api_ascwin,_api_picwin				;窗口创建API接口

[SECTION .text]

_api_printc:		; void api_printc(int c);
	MOV EDX,1		;开关1，打印单个字符
	MOV AL,[ESP+4] 	;传字符
	INT 0x40		;API中断
	RET				;返回调用者
		
_api_return:		;void api_return();
	MOV EDX,4		;开关4，结束应用并返回
	INT 0x40
	
_api_printl:		;void api_printl(char *s);
	PUSH EBX		;保存寄存器信息
	MOV EDX,2		;开关2，打印一行字符串
	MOV EBX,[ESP+8]	;传字符串
	INT	0x40		;API中断
	POP EBX			;恢复寄存器信息
	RET				;返回调用者
	
_api_window:		;int api_window(char *buffer,int width,int height,int vcolor,char *caption);
	 PUSH	EDI
	 PUSH	ESI
	 PUSH	EBX
	 MOV EDX,5
	 MOV EBX,[ESP+16] 	;buffer
	 MOV ESI,[ESP+20] 	;width
	 MOV EDI,[ESP+24] 	;height
	 MOV EAX,[ESP+28] 	;vcolor
	 MOV ECX,[ESP+32] 	;caption
	 INT 0x40
	 POP EBX
	 POP ESI
	 POP EDI
	 RET

_api_ascwin: ;void api_ascwin(int win,int x,int y,int vcolor,int len,char *str);
	PUSH EDI
	PUSH ESI
	PUSH EBP
	PUSH EBX
	MOV  EDX,6 ;API入口开关
	MOV  EBX,[ESP+20] ;win
	MOV  ESI,[ESP+24] ;x
	MOV  EDI,[ESP+28] ;y
	MOV  EAX,[ESP+32] ;col
	MOV  ECX,[ESP+36] ;len
	MOV  EBP,[ESP+40] ;str
	INT  0x40
	POP  EBX
	POP  EBP
	POP  ESI
	POP  EDI
	RET

_api_picwin: ;void api_picwin(int win,int x0,int y0,int x1,int y1,int vcolor);
	PUSH EDI
	PUSH ESI
	PUSH EBP
	PUSH EBX
	MOV  EDX,7 ;API入口开关
	MOV  EBX,[ESP+20] ;win
	MOV  EAX,[ESP+24] ;x0
	MOV  ECX,[ESP+28] ;y0
	MOV  ESI,[ESP+32] ;x1
	MOV  EDI,[ESP+36] ;y1
	MOV  EBP,[ESP+40] ;vcolor
	INT  0x40
	POP  EBX
	POP  EBP
	POP  ESI
	POP  EDI
	RET 
	
