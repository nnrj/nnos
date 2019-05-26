[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api025.nas"]

		GLOBAL	_api_fread

[SECTION .text]

_api_fread:       		;int api_fread(char *buffer, int maxsize, int fileHanle);
	PUSH EBX
	MOV  EDX,25
	MOV  EAX,[ESP+16]	;fileHanle
	MOV  ECX,[ESP+12]  	;maxsize
	MOV  EBX,[ESP+8]   	;buffer
	INT  0x40
	POP  EBX
	RET
