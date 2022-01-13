[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api004.nas"]

		GLOBAL	_api_return

[SECTION .text]

_api_return:	; void api_return(void);
		MOV		EDX,4
		INT		0x40
