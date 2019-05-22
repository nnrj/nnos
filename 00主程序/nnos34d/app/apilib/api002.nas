[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api002.nas"]

		GLOBAL	_api_printl

[SECTION .text]

_api_printl:	; void api_printl(char *s);
		PUSH	EBX
		MOV		EDX,2
		MOV		EBX,[ESP+8]		; s
		INT		0x40
		POP		EBX
		RET
