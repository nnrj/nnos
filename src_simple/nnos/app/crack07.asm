;应用程序段越权病毒
;[FORMAT "WCOFF"]
;[INSTRSET "i486p"]
[BITS 32]
;[FILE "crack07.nas"]

		GLOBAL	_NNOSMain

[SECTION .text]

_NNOSMain:
		MOV		AX,1005*8
		MOV		DS,AX
		CMP		DWORD [DS:0x0004],'NNOS'
		JNE		fin					

		MOV		ECX,[DS:0x0000]
		MOV		AX,2005*8
		MOV		DS,AX

crackloop:	
		ADD		ECX,-1
		MOV		BYTE [DS:ECX],123
		CMP		ECX,0
		JNE		crackloop

fin:
		MOV		EDX,4
		INT		0x40
