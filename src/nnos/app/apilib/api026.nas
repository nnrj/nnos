[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api026.nas"]

		GLOBAL	_api_command

[SECTION .text]

_api_command:				;int api_cmdline(char *buffer, int maxSize);
	PUSH EBX
	MOV EDX,26
	MOV ECX,[ESP+12]		;maxsize
	MOV EBX,[ESP+8]			;buffer
	INT 0x40
	POP EBX
	RET
