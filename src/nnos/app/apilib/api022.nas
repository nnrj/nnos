[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api022.nas"]

		GLOBAL	_api_fclose

[SECTION .text]

_api_fclose:      		;void api_fclose(int fileHanle);
	MOV  EDX,22
	MOV  EAX,[ESP+4]   	;fileHanle
	INT  0x40
	RET
	