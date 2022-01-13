[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "alloca.nas"]

		GLOBAL	__alloca

[SECTION .text]

__alloca:
	ADD EAX,-4
	SUB ESP,EAX
	JMP DWORD [ESP+EAX] ;代替RET
	
;产生在下列情况被C语言程序调用
	;执行从栈中分配EAX个字节内存空间（ESP -=EAX）
	;要遵守的规则不能改变ECX、EDX、EBX、EBP、ESI、EDI的值（改变须用PUSH和POP）
	;下列写法错误
	;SUB ESP,EAX 
	;RET //因为RET相当于POP EIP，RET返回的地址保存在了ESP中，在上面被改变了
	;下列写法也错误
	;SUB ESP,EAX
	;JMP DWORD [ESP+EAX] ;//代替RET
	;//RET相当于POP EIP,POP IP相当于MOV EIP,[EIP]		ADD ESP,4; //ESP的值有偏差
	;下列写法仍然错误
	;SUB ESP,EAX
	;SUB DWORD [ESP+EAX] ;//代替RET
	;ADD ESP,4 ;//ADD指令位置，将ADD放在了JMP后面，不可能执行
	;下列写法基本正确
	;SUB ESP,EAX
	;ADD ESP,4
	;JMP DWORD [ESP+EAX-4]
	;实际的写法更加简短
