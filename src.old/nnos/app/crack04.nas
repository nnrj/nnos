;HLT病毒
[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "crack04.nas"]

	GLOBAL	_NNOSMain

[SECTION .text]

_NNOSMain:
	CLI 	;关中断
fin:
	HLT 	;待机
	JMP fin ;无限循环
