;[FORMAT "WCOFF"]
;[INSTRSET "i486p"]				
[BITS 32]						
;[FILE "api.asm"]

GLOBAL _api_printc,_api_printl,_api_printx								;字符输出API接口

[SECTION .text]

;字符输出======================================================
_api_printc:		; void api_printc(int c);
	MOV EDX,1		;开关1，打印单个字符
	MOV AL,[ESP+4] 	;传字符
	INT 0x40		;API中断
	RET				;返回调用者
	
_api_printl:		;void api_printl(char *s);
	PUSH EBX		;保存寄存器信息
	MOV EDX,2		;开关2，打印一行字符串
	MOV EBX,[ESP+8]	;传字符串
	INT	0x40		;API中断
	POP EBX			;恢复寄存器信息
	RET				;返回调用者
	
_api_printx:		;void api_printx(char *s,int l);
		PUSH	EBX
		MOV		EDX,3
		MOV		EBX,[ESP+ 8]	; s
		MOV		ECX,[ESP+12]	; l
		INT		0x40
		POP		EBX
		RET