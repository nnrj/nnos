[FOMAT "WCOFF"]		;生成obj文件
[INSTRSET "i486p"] 	;使用486指令集
[BITS 32] 			;32位模式机器语言
[FILE "api_asm.nas"]	;源文件

	GLOBAL _api_sysprint
[SECTION .text]

_api_sysprint:	;void api_syspirnt(int c);
	MOV EDX,1		;功能号置1
	MOV AL,[ESP+4]	;一个字节字符作为参数，即c
	INT 0x40		;中断
	RET				;返回调用者
	