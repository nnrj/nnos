[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api003.nas"]

		GLOBAL	_api_printx

[SECTION .text]

_api_printx:	; void api_printx(char *s, int l);
		PUSH	EBX
		MOV		EDX,3
		MOV		EBX,[ESP+ 8]	; s
		MOV		ECX,[ESP+12]	; l
		INT		0x40
		POP		EBX
		RET
