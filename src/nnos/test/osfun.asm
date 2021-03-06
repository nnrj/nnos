;nnos_osfun（系统底层函数库）

;[FORMAT "WCOFF"]     ;制作目标文件的模式为"WCOFF"。兼容nask。nasm中注释掉,nask中去掉注释。
;[INSTRSET "i486p"] ;指定80486处理器模式编译,但是生成的机器语言同样可以在386下运行（本程序未涉及486特有指令）。兼容nask。
;[BITS 32]           ;制作32位模式用的机器语言。兼容nask。

;制作目标文件信息
;[FILE "osfun.nas"]     ;源文件信息。兼容nask。
	;GLOBAL _write_mem8 	;(废弃，使用C语言指针代替)向指定内存地址写入数据的函数声明,本函数在0.7f中已由C语言代替,废弃（最终编译时注释掉）,末尾数字8指明一次写入8位
	GLOBAL _io_hlt,_io_cli,_io_sti,_io_stihlt  	;公用函数声明,函数名前加下划线用于与C语言连接。从左到右依次为停机等待、屏蔽中断、允许中断、中断并等待
	GLOBAL _io_in8,_io_in16,_io_in32 			;从指定IO设备端口读取数据,从左到右依次为每次读取8位、16位和32位
	GLOBAL _io_out8,_io_out16,_io_out32			;向指定IO设备端口写入数据,从左到右依次为每次写入8位、16位和32位
	GLOBAL _io_store_eflags,_io_load_eflags	;左为保护现场,存储EFLAGS标志位信息；右为恢复现场,读取标志位信息到EFLAGS
    GLOBAL _load_gdtr,_load_idtr
	GLOBAL _asm_inthandler20,_asm_inthandler21, _asm_inthandler27, _asm_inthandler2c,_asm_inthandler0d,_asm_inthandler0c
	EXTERN _inthandler20,_inthandler21, _inthandler27, _inthandler2c,_inthandler0d,_inthandler0c
	GLOBAL _load_cr0,_store_cr0
	GLOBAL _memtest_sub
	GLOBAL _load_tr,_taskswitch3,_taskswitch4,_farjmp
	;GLOBAL _asm_sysprint,_farcall,_asm_sys_api
	;EXTERN _sysprint,_sys_api
	GLOBAL _farcall,_asm_sys_api,_run_app,_asm_end_app
	EXTERN _sys_api	
	GLOBAL _asm_shutdown ;关机
;函数实现
;[SECTION .text]	;编译时先编译此部分。本函数实现类似汇编指令"HLT"(CUP停机等待)的功能,目的是节约用电。兼容nask。
_io_hlt:			;函数名标号,函数调用了HLT指令,实现了CUP停机等待。void io_hlt();
    HLT             ;调用CPU停机等待指令
    RET             ;返回调用者

_io_cli:			;void io_cli();
	CLI				;屏蔽中断,CLI与间的程序段不会被外部中断打断（内部中断仍可能打断）
	RET				;返回调用者

_io_sti:			;void io_stl():
	STI 			;允许中断
	RET				;返回调用者

_io_stihlt:			;允许中断并停机等待。void io_stihlt();
	STI				;允许中断
	HLT 			;停机等待
	RET 			;返回调用者

_io_in8:			;int io_in8(int port);
	MOV EDX,[ESP+4]	;端口号prot写入EDX
	MOV EAX,0		;初始化EAX,置0
	IN	AL,DX		;DX的内容存写入16位累加器的低8位
	RET 			;返回调用者

_io_in16:			;int io_int16(int port);
	MOV EDX,[ESP+4]	;端口号prot写入EDX
	MOV EAX,0		;初始化EAX,置0
	IN	AX,DX		;DX的内容存写入16位累加器,二者结尾16位寄存器
	RET 			;返回调用者	

_io_in32:			;int io_int32(int port);
	MOV EDX,[ESP+4]	;端口号prot写入EDX
	;MOV EAX,0		;初始化EAX,置0
	IN	EAX,DX		;DX的内容存写入32位累加器EAX
	RET 			;返回调用者

_io_out8:			;void io_out8(int port, int data);
	MOV EDX,[ESP+4]	;端口号prot写入EDX
	MOV AL,[ESP+8]	;偏移位即一个字节写入累加器的低8位
	;MOV DX,0		;初始化DX寄存器,置0
	OUT DX,AL 		;将8位寄存器AL中的数据写入DX寄存器,DX高8位不变
	RET 			;返回调用者
_io_out16:			;void io_out16();
	MOV EDX,[ESP+4]	;端口号prot写入EDX
	MOV EAX,[ESP+8]	;偏移位即一个字节写入累加器
	;MOV DX,0		;初始化DX寄存器,置0
	OUT DX,AX 		;将16位寄存器AX中的数据写入DX寄存器
	RET 			;返回调用者
_io_out32:			;void io_out32();
	MOV EDX,[ESP+4]	;端口号prot写入EDX
	MOV EAX,[ESP+8]	;偏移位即一个字节写入累加器的低8位
	;MOV DX,0		;初始化DX寄存器,置0
	OUT DX,EAX 		;将EAX中的数据写入DX寄存器
	RET 			;返回调用者

;CPU没有类似MOV EAX,EFLAGS的指令,只能通过出入栈操作来读写EFLAGS,完成保护现场和恢复现场
_io_store_eflags:	;void io_store_eflgs();
	MOV EAX,[ESP+4] ;栈顶指针偏移4位处的地址所存数据写入EAX
	PUSH EAX 		;压栈,将EAX所存内容入栈
	POPFD 			;出栈,将标志位按双字长从栈弹出。相当于nasm的 POP EFLAGS。"POPFD"是"pop flags double-word"的缩写,nask特有语法。
	RET				;返回调用者,此函数有返回值,执行RET时便以将EAX的值返回（规定）


_io_load_eflags:	;void load_eflags(int eflags);
	PUSHFD			;压栈,将表示为值按双字长入栈。相当于nasm的 PUSH EFLAGS。"PUSHFD"是"push flags double-word"的缩写,nask特有语法。
	POP	EAX 		;EAX顶部数据出栈
	RET 			;返回调用者,此函数有返回值,执行RET时便以将EAX的值返回（规定）

_load_gdtr:			;void load_gdtr(int limit,int addr);加载全局描述符到全局描述符寄存器。DWORD[ESP+4]中为段上限(0x0000ffff)，DWORD[ESP+8]中为首地址(0x00270000)。
	MOV AX,[ESP+4]	;将地址以addr为首地址的段的段上限写入段寄存器GDTR，此寄存器大小为48位。读取4位。
	MOV [ESP+6],AX 	;GDTR为专用寄存器，无法用MOV直接赋值，只能先写入AX，指定一个内存地址，然后从此地址读取（ESP+6，栈指针偏移6个字节，即48位)。
	LGDT [ESP+6]	;LGDT命令，将值写入GDTR。GDTR低16位为段上限(=GDT有效字节数-1),高32位(剩余4字节)为GDT首地址
	RET				;返回调用者。写入前：[FF FF 00 00 00 27 00]，小端模式（低地址，低字节）。写入后：[FF FF FF FF 00 27 00 00]

_load_idtr:			;加载中断描述符到寄存器;void load_idtr(int limit,int addr);
	MOV AX,[ESP+4]	;将地址以addr为首地址的段的段上限写入段寄存器IDTR，此寄存器大小为48位。
	MOV [ESP+6],AX	;IDTR为专用寄存器，无法用MOV直接赋值，只能先写入AX，指定一个内存地址，然后从此地址读取（ESP+6，栈指针偏移6个字节，即48位)。	
	LIDT [ESP+6]	;LIDT命令，将值写入IDTR。IDTR低16位为段上限(=IDT有效字节数-1),高32位(剩余4字节)为IDT首地址
	RET 
				;返回调用者
				
_asm_inthandler21:				;保护键盘中断现场，寄存器信息入栈，调用中断处理函数，恢复寄存器信息，执行IRETD
	PUSH ES				
	PUSH DS
	PUSHAD					;push all data,PUSH EAX;PUSH ECX;PUSH EDX;PUSH EBX;PUSH ESP;PUSH EBP;PUSH ESI;PUS EDI;
	MOV EAX,ESP
	PUSH EAX				;ADD ESP,-4;MOV [SS:ESP],EAX
	MOV AX,SS			;C语言认为DS、ES、SS都指同一个段
	MOV DS,AX
	MOV ES,AX
	CALL _inthandler21	;调用inthandler21()函数（见interrupt.c)，产生使用EXTERN指令声明、引入
	POP EAX				;MOV EXA,[SS:ESP];ADD ESP,4
	POPAD					;pop all data
	POP DS
	POP ES
	IRETD

; _asm_inthandler21:				;保护键盘中断现场，寄存器信息入栈，调用中断处理函数，恢复寄存器信息，执行IRETD
	; PUSH ES
	; PUSH DS
	; PUSHAD					;push all data,PUSH EAX;PUSH ECX;PUSH EDX;PUSH EBX;PUSH ESP;PUSH EBP;PUSH ESI;PUS EDI;
	; MOV  AX,SS
	; CMP  AX,1*8
	; JNE  .from_app

	; MOV  EAX,ESP
	; PUSH SS	
	; PUSH EAX 			;ADD ESP,-4;MOV [SS:ESP],EAX       
	; MOV  AX,SS			;C语言认为DS、ES、SS都指同一个段
	; MOV  DS,AX
	; MOV  ES,AX
	; CALL _inthandler21	;调用inthandler21()函数（见interrupt.c)，产生使用EXTERN指令声明、引入
	; ADD  ESP,8
	; POPAD
	; POP  DS
	; POP  ES
	; IRETD					;中断处理完成后不能执行return/RET，须执行IRETD指令，此指令C语言无法完成
	
; .from_app:
	; MOV  EAX,1*8
	; MOV  DS,AX   
	; MOV  ECX,[0xfe4]  
	; ADD  ECX,-8
	; MOV  [ECX+4],SS  
	; MOV  [ECX  ],ESP  
	; MOV  SS,AX
	; MOV  ES,AX
	; MOV  ESP,ECX
	; CALL _inthandler21
	; POP  ECX
	; POP  EAX
	; MOV  SS,AX   
	; MOV  ESP,ECX   
	; POPAD
	; POP  DS
	; POP  ES
	; IRETD

_asm_inthandler27:				;保护自动中断现场
	PUSH	ES
	PUSH	DS
	PUSHAD
	MOV		EAX,ESP
	PUSH	EAX
	MOV		AX,SS
	MOV		DS,AX
	MOV		ES,AX
	CALL	_inthandler27
	POP		EAX
	POPAD
	POP		DS
	POP		ES
	IRETD

; _asm_inthandler27:
	; PUSH ES
	; PUSH DS
	; PUSHAD
	; MOV  AX,SS
	; CMP  AX,1*8
	; JNE  .from_app

	; MOV  EAX,ESP
	; PUSH SS    
	; PUSH EAX    
	; MOV  AX,SS
	; MOV  DS,AX
	; MOV  ES,AX
	; CALL _inthandler27
	; ADD  ESP,8
	; POPAD
	; POP  DS
	; POP  ES
	; IRETD

; .from_app:
	; MOV  EAX,1*8
	; MOV  DS,AX   
	; MOV  ECX,[0xfe4]  
	; ADD  ECX,-8
	; MOV  [ECX+4],SS  
	; MOV  [ECX  ],ESP  
	; MOV  SS,AX
	; MOV  ES,AX
	; MOV  ESP,ECX
	; CALL _inthandler27
	; POP  ECX
	; POP  EAX
	; MOV  SS,AX   
	; MOV  ESP,ECX   
	; POPAD
	; POP  DS
	; POP  ES
	; IRETD
	
_asm_inthandler2c:			 ;保护鼠标中断现场
	PUSH	ES
	PUSH	DS
	PUSHAD
	MOV		EAX,ESP
	PUSH	EAX
	MOV		AX,SS
	MOV		DS,AX
	MOV		ES,AX
	CALL	_inthandler2c
	POP		EAX
	POPAD
	POP		DS
	POP		ES
	IRETD

; _asm_inthandler2c:
	; PUSH ES
	; PUSH DS
	; PUSHAD
	; MOV  AX,SS
	; CMP  AX,1*8
	; JNE  .from_app

	; MOV  EAX,ESP
	; PUSH SS    
	; PUSH EAX    
	; MOV  AX,SS
	; MOV  DS,AX
	; MOV  ES,AX
	; CALL _inthandler2c
	; ADD  ESP,8
	; POPAD
	; POP  DS
	; POP  ES
	; IRETD
	
; .from_app:
	; MOV  EAX,1*8
	; MOV  DS,AX   
	; MOV  ECX,[0xfe4]  
	; ADD  ECX,-8
	; MOV  [ECX+4],SS  
	; MOV  [ECX  ],ESP  
	; MOV  SS,AX
	; MOV  ES,AX
	; MOV  ESP,ECX
	; CALL _inthandler2c
	; POP  ECX
	; POP  EAX
	; MOV  SS,AX   
	; MOV  ESP,ECX   
	; POPAD
	; POP  DS
	; POP  ES
	; IRETD

_asm_inthandler20:
	PUSH ES
	PUSH DS
	PUSHAD
	MOV EAX,ESP
	PUSH EAX
	MOV AX,SS
	MOV DS,AX
	MOV ES,AX
	CALL _inthandler20
	POP EAX
	POPAD
	POP DS
	POP ES
	IRETD
	
; _asm_inthandler20:
	; PUSH ES
	; PUSH DS
	; PUSHAD
	; MOV  AX,SS
	; CMP  AX,1*8
	; JNE  .from_app

	; MOV  EAX,ESP
	; PUSH SS    
	; PUSH EAX    
	; MOV  AX,SS
	; MOV  DS,AX
	; MOV  ES,AX
	; CALL _inthandler20
	; ADD  ESP,8
	; POPAD
	; POP  DS
	; POP  ES
	; IRETD
	; .from_app:

	; MOV  EAX,1*8
	; MOV  DS,AX   
	; MOV  ECX,[0xfe4]  
	; ADD  ECX,-8
	; MOV  [ECX+4],SS  
	; MOV  [ECX  ],ESP  
	; MOV  SS,AX
	; MOV  ES,AX
	; MOV  ESP,ECX
	; CALL _inthandler20
	; POP  ECX
	; POP  EAX
	; MOV  SS,AX   
	; MOV  ESP,ECX   
	; POPAD
	; POP  DS
	; POP  ES
	; IRETD	
	
; _asm_inthandler0d:
	; STI
	; PUSH	ES
	; PUSH	DS
	; PUSHAD
	; MOV AX,SS
	; CMP AX,1*8
	; JNE .from_app
	; MOV EAX,ESP
	; PUSH	SS  
	; PUSH	EAX  
	; MOV AX,SS
	; MOV DS,AX
	; MOV ES,AX
	; CALL	_inthandler0d
	; ADD ESP,8
	; POPAD
	; POP DS
	; POP ES
	; ADD ESP,4 	
	; IRETD
	
; .from_app:
	; CLI
	; MOV EAX,1*8
	; MOV DS,AX 	
	; MOV ECX,[0xfe4] 
	; ADD ECX,-8
	; MOV [ECX+4],SS 
	; MOV [ECX  ],ESP 
	; MOV SS,AX
	; MOV ES,AX
	; MOV ESP,ECX
	; STI
	; CALL	_inthandler0d
	; CLI
	; CMP EAX,0
	; JNE .kill
	; POP ECX
	; POP EAX
	; MOV SS,AX 	
	; MOV ESP,ECX 	
	; POPAD
	; POP DS
	; POP ES
	; ADD ESP,4 	
	; IRETD
	
; .kill:
	; MOV EAX,1*8 	
	; MOV ES,AX
	; MOV SS,AX
	; MOV DS,AX
	; MOV FS,AX
	; MOV GS,AX
	; MOV ESP,[0xfe4] 
	; STI 	
	; POPAD	
	; RET
	
_asm_inthandler0d: ;一般异常中断处理
	STI
	PUSH ES
	PUSH DS
	PUSHAD
	MOV EAX,ESP
	PUSH EAX
	MOV AX,SS
	MOV DS,AX
	MOV ES,AX
	CALL _inthandler0d
	CMP EAX,0
	JNE _asm_end_app
	POP EAX
	POPAD
	POP DS
	POP ES
	ADD ESP,4 ;INT 0x0d
	IRETD
	
_asm_inthandler0c: ;栈异常中断处理
	STI
	PUSH ES
	PUSH DS
	PUSHAD
	MOV EAX,ESP
	PUSH EAX
	MOV AX,SS
	MOV DS,AX
	MOV ES,AX
	CALL _inthandler0c
	CMP EAX,0
	JNE _asm_end_app
	POP EAX
	POPAD
	POP DS
	POP ES
	ADD ESP,4
	IRETD
		
_load_cr0:					;int load_cr0();
	MOV EAX,CR0				;将寄存器CRO中的数据写入EAX
	RET						;返回调用者

_store_cr0:					;void store_cr0(int cr0);
	MOV EAX,[ESP+4]			;将栈顶指针偏移4，所指数据存取EAX
	MOV CR0,EAX 			;将EAX中的数据写入CR0。CRO不能直接赋值。
	RET 					;返回调用者
	
_memtest_sub:				;unsigned int memtest_sub(unsigned int start,unsigned int end);
	PUSH EDI 
	PUSH ESI
	PUSH EBX
	MOV ESI,0xaa55aa55
	MOV EDI,0x55aa55aa
	MOV EAX,[ESP+12+4]
	
mts_loop:
	MOV EBX,EAX
	ADD EBX,0xffc				;p=i+0xff0;
	MOV EDX,[EBX]				;0ld=*p;
	MOV [EBX],ESI				;*p=pat0
	XOR DWORD [EBX],0xffffffff	;*p^=0xffffffff;
	CMP EDI,[EBX]				;if(*p!=pat1)goto fin;
	JNE mts_fin
	XOR DWORD [EBX],0xffffffff	;*^=0xffffffff
	CMP ESI,[EBX]				;if(*p!=pat0)goto fin;
	JNE mts_fin 
	MOV [EBX],EDX				;*p=old
	ADD EAX,0x1000				;i+=0x1000
	CMP EAX,[ESP+12+8]			;if(i<=end) goto mts_loop;
	JBE mts_loop
	POP EBX
	POP ESI
	POP EDI
	RET

mts_fin:
	MOV [EBX],EDX 
	POP EBX
	POP ESI 
	POP EDI 
	RET 
	
_load_tr:		;void load_tr(int tr);
	LTR [ESP+4] ;改变TR寄存器的值，不会发生任务切换
	RET 
	
_taskswitch3: ;void taskswitch3();
	JMP 3*8:0 ;执行far模式跳转指令，进行任务切换。冒号前的部分指向TSS，后面的部分无实际作用
	RET		  ;返回调用者	
	
_taskswitch4: ;void taskswitch4();
	JMP 4*8:0 ;执行far模式跳转指令，进行任务切换。冒号前的部分指向TSS，后面的部分无实际作用
	RET		  ;返回调用者

_farjmp:			;void farjmp(int esp,int cs);调用此函数是，[ESP + 4]的位置存放了EIP的值，[ESP+8]位置存放了CS的值，从而实现far跳转
	JMP FAR[ESP+4] 	;执行far跳转，可指定内存地址，CPU从此地址读取4字节数据，并写入EIP，再继续读2字节数据，写入CS
	RET			

; _asm_sysprint:
	; STI ;开中断
	; PUSHAD	;保存寄存器值
	; PUSH 1
	; AND EAX,0xff ;AH与EAX高位置0，EAX置为已存入字符编码的状态
	; PUSH EAX
	; PUSH DWORD [0x0fec] ;读取函数所在地址并入栈
	; CALL _sysprint
	; ADD ESP,12 ;丢弃栈中数据
	; ;RET ;返回调用者，考虑到段号，用far-RET，即RETF
	; ;RETF
	; POPAD ;还原寄存器值
	; IRETD ;返回中断调用者
	
; _asm_sys_api:
	; STI ;开中断
	; PUSHAD	;保存寄存器值的push
	; PUSHAD	;向sys_api传值的push
	; CALL _sys_api ;调用系统API处理函数
	; ADD ESP,32 ;丢弃栈中数据
	; POPAD ;还原寄存器值
	; IRETD ;返回中断调用者	
	
; _asm_sys_api:
	; PUSH DS
	; PUSH ES
	; PUSHAD			;保存寄存器值
	; MOV EAX,1*8		;操作系统所在段
	; MOV DS,AX		;将DS设为OS用
	; MOV ECX,[0xfe4] ;操作新esp
	; ADD ECX,-40
	; MOV [ECX+32],ESP ;保存应用程序esp
	; MOV [ECX+36],SS ;保存应用程序ss
	
	; MOV		EDX,[ESP]
	; MOV		EBX,[ESP+4]
	; MOV		[ECX],EDX	;传值给sys_api(下同)
	; MOV		[ECX+4],EBX
	; MOV		EDX,[ESP+8]
	; MOV		EBX,[ESP+12]
	; MOV		[ECX+8],EDX
	; MOV		[ECX+12],EBX
	; MOV		EDX,[ESP+16]
	; MOV		EBX,[ESP+20]
	; MOV		[ECX+16],EDX
	; MOV		[ECX+20],EBX
	; MOV		EDX,[ESP+24]
	; MOV		EBX,[ESP+28]
	; MOV		[ECX+24],EDX
	; MOV		[ECX+28],EBX
	; MOV		ES,AX			;剩余段寄存器设为OS用
	; MOV		SS,AX
	; MOV		ESP,ECX
	; STI						;开中断
	; CALL	_sys_api
	; MOV		ECX,[ESP+32]
	; MOV		EAX,[ESP+36]
	; CLI						;关中断
	; MOV		SS,AX
	; MOV		ESP,ECX
	; POPAD
	; POP		ES
	; POP		DS
	; IRETD					;返回（自动开中断）
	
_asm_sys_api:
	STI
	PUSH DS
	PUSH ES
	PUSHAD
	PUSHAD
	MOV  AX,SS
	MOV  DS,AX  
	MOV  ES,AX
	CALL _sys_api
	CMP  EAX,0  
	JNE  _asm_end_app
	ADD  ESP,32
	POPAD
	POP  ES
	POP  DS
	IRETD

; _asm_end_app:
	; MOV  ESP,[EAX]
	; POPAD
	; RET
	
_asm_end_app:
	MOV ESP,[EAX]
	MOV DWORD [EAX+4],0 ;运行状态标志位置0
	POPAD
	RET					;返回
	
_farcall: ;void farcall(int eip,int cs); 用于在C语言中调用应用程序
	CALL FAR [ESP+4] ;esp,cs
	RET
	
; _run_app:			 ;void run_app(int eip,int cs,int esp,int ds);
	; PUSHAD 			 ;保存32位寄存器值
	; MOV EAX,[ESP+36] ;为寄存器赋值（应用程序所需参数）
	; MOV ECX,[ESP+40]
	; MOV EDX,[ESP+44]
	; MOV EBX,[ESP+48]
	; MOV [0xfe4],ESP
	; CLI				;关中断，切换到应用程序
	; MOV ES,BX
	; MOV SS,BX
	; MOV DS,BX
	; MOV FS,BX
	; MOV GS,BX
	; MOV ESP,EDX
	; STI 			;切换完成开中断
	; PUSH ECX 		;farcall,push(cs)
	; PUSH EAX 		;farcall,push(eip)
	; CALL FAR [ESP]  ;调用应用程序
	
	; MOV EAX,1*8   	;开始返回操作系统
	; CLI 		 	;关中断
	; MOV ES,AX
	; MOV SS,AX
	; MOV DS,AX
	; MOV FS,AX
	; MOV GS,AX
	; MOV ESP,[0xfe4]
	; STI				;开中断
	; POPAD			;恢复寄存器信息
	; RET				;返回
	
_run_app:			 ;void run_app(int eip,int cs,int esp,int ds,int *tss_esp0);  
	PUSHAD  
	MOV  EAX,[ESP+36] 
	MOV  ECX,[ESP+40] 
	MOV  EDX,[ESP+44] 
	MOV  EBX,[ESP+48] 
	MOV  EBP,[ESP+52] 
	MOV  [EBP  ],ESP  
	MOV  [EBP+4],SS  
	MOV  ES,BX
	MOV  DS,BX
	MOV  FS,BX
	MOV  GS,BX
	OR  ECX,3   
	OR  EBX,3   
	PUSH EBX    
	PUSH EDX    
	PUSH ECX    
	PUSH EAX    
	RETF
	
; _asm_shutdown: 		;void asm_shutdown();
	; MOV DX,805h
	; MOV AL,3ch
	; OUT DX,AL
	; RET
	
_asm_shutdown: 		;void asm_shutdown();
	;INT 15h
	; MOV DX,805h
	; MOV AL,3ch
	; OUT DX,AL
	RET

;=====已废弃的函数============================================================================================
;内存数据写入函数(废弃，使用C语言指针代替)
;_write_mem8:            ;相当于C语言的viod write_mem8(int addr,int data);通过此函数传入地址和数据,地址占4位,数据占8位,以此类推
;	MOV ECX,[ESP+4]     ;栈顶寄存器ESP中的地址偏移4位ESP+4中存放着地址,将此地址写入寄存器ECX；在32位模式应尽量使用32位寄存器,避免机器语言字节数增加、执行速度减慢
;	MOV AL,[ESP+8]      ;SP+8中存放着数据,将数据写入寄存器AL；也可使用寄存器加常数指定地址
;	MOV [ECX],AL        ;将AL的数据写入ECX中的地址；32位模式支持使用ECX和ESP指定内存
;	RET                 ;返回调用者；与C语言混合编译,只有EAX、ECX和EDX可以自由使用,其他的可以取值,不能改变（比如ESP在C语言生成的机器语言中存放栈顶指针,任意改变则出错）
