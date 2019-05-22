;定时器病毒
;[INSTRSET "i486"]
[BITS 32]
	MOV AL,0x34
	OUT 0x43,AL
	MOV AL,0xff
	OUT 0x40,AL
	MOV AL,0xff
	OUT 0x40,AL
;相当于
	;io_out8(PIT_CTRL,0x34)
	;io_out8(PIT_CNT0,0xff)
	;io_out8(PIT_CNT0,0xff)
return:
	MOV EDX,4
	INT 0x40