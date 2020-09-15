;HLT病毒
;[INSTRSET "i486"]
[BITS 32]
	CLI 	;关中断
fin:
	HLT 	;待机
	JMP fin ;无限循环