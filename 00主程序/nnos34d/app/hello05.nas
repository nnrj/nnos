[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "hello5.nas"]

	GLOBAL	_NNOSMain

[SECTION .text]

_NNOSMain:
	MOV EDX,2
	MOV EBX,msg
	INT 0x40
	MOV EDX,4
	INT 0x40

[SECTION .data]

msg:
	;DB	"Hello world agin5, nnos!", 0x0a, 0 ;输出字符串并换行
	DB	"Hello world agin5, nnos!", 0 ;输出字符串不换行