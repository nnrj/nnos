[FORMAT "WCOFF"]
[INSTRSET "i486p"]				
[BITS 32]						
[FILE "api.nas"]				

	GLOBAL _api_printc,_api_printl,_api_printx								;字符输出API接口
	GLOBAL _api_window,_api_ascwin,_api_boxwin,_api_closewin				;窗口创建API接口
	GLOBAL _api_initmalloc,_api_malloc,_api_free							;内存创建API接口
	GLOBAL _api_point,_api_linewin											;图形绘制API接口
	GLOBAL _api_refreshwin													;图层处理API接口
	GLOBAL _api_return														;控制相关API接口
	GLOBAL _api_getkey														;输入相关API接口
	GLOBAL _api_alloctimer,_api_inittimer,_api_settimer,_api_freetimer		;内存相关API接口
	GLOBAL _api_beep														;蜂鸣发声API接口
	GLOBAL _api_fopen,_api_fclose,_api_fseek,_api_fsize,_api_fread			;文件处理API接口
	GLOBAL _api_command														;控制台之API接口
	GLOBAL _api_getlang														;字体相关API接口

[SECTION .text]

;字符输出======================================================
_api_printc:		; void api_printc(int c);
	MOV EDX,1		;开关1，打印单个字符
	MOV AL,[ESP+4] 	;传字符
	INT 0x40		;API中断
	RET				;返回调用者
	
_api_printl:		;void api_printl(char *s);
	PUSH EBX		;保存寄存器信息
	MOV EDX,2		;开关2，打印一行字符串
	MOV EBX,[ESP+8]	;传字符串
	INT	0x40		;API中断
	POP EBX			;恢复寄存器信息
	RET				;返回调用者
	
_api_printx:		;void api_printx(char *s,int l);
		PUSH	EBX
		MOV		EDX,3
		MOV		EBX,[ESP+ 8]	; s
		MOV		ECX,[ESP+12]	; l
		INT		0x40
		POP		EBX
		RET

;图形绘制======================================================	
_api_boxwin: ;void api_picwin(int win,int x0,int y0,int x1,int y1,int vcolor);
	PUSH EDI
	PUSH ESI
	PUSH EBP
	PUSH EBX
	MOV  EDX,7 ;API入口开关
	MOV  EBX,[ESP+20] ;win
	MOV  EAX,[ESP+24] ;x0
	MOV  ECX,[ESP+28] ;y0
	MOV  ESI,[ESP+32] ;x1
	MOV  EDI,[ESP+36] ;y1
	MOV  EBP,[ESP+40] ;vcolor
	INT  0x40
	POP  EBX
	POP  EBP
	POP  ESI
	POP  EDI
	RET 

_api_point:			;void api_point(int win,int x,int y,int vcolor);
	PUSH EDI
	PUSH ESI
	PUSH EBX
	MOV EDX,11
	MOV EBX,[ESP+16] ;win
	MOV ESI,[ESP+20] ;x
	MOV EDI,[ESP+24] ;y
	MOV EAX,[ESP+28] ;vcolor
	INT 0x40 ;API中断
	POP EBX
	POP ESI
	POP EDI
	RET
	
;窗口相关======================================================	
_api_window:		;int api_window(char *buffer,int width,int height,int vcolor,char *caption);
	 PUSH	EDI
	 PUSH	ESI
	 PUSH	EBX
	 MOV EDX,5
	 MOV EBX,[ESP+16] 	;buffer
	 MOV ESI,[ESP+20] 	;width
	 MOV EDI,[ESP+24] 	;height
	 MOV EAX,[ESP+28] 	;vcolor
	 MOV ECX,[ESP+32] 	;caption
	 INT 0x40
	 POP EBX
	 POP ESI
	 POP EDI
	 RET
	 
_api_ascwin: ;void api_ascwin(int win,int x,int y,int vcolor,int len,char *str);
	PUSH EDI
	PUSH ESI
	PUSH EBP
	PUSH EBX
	MOV  EDX,6 ;API入口开关
	MOV  EBX,[ESP+20] ;win
	MOV  ESI,[ESP+24] ;x
	MOV  EDI,[ESP+28] ;y
	MOV  EAX,[ESP+32] ;col
	MOV  ECX,[ESP+36] ;len
	MOV  EBP,[ESP+40] ;str
	INT  0x40
	POP  EBX
	POP  EBP
	POP  ESI
	POP  EDI
	RET

;EBX-win,EAX-x0,ECX-y0,ESI-x1,EDI-y1,EDX-12
_api_refreshwin: 	;void api_refreshwin(int win,int x0,int y0,int x1,int y1);
	PUSH EDI	;入栈，保护现场
	PUSH ESI	
	PUSH EBX	
	MOV EDX,12	;API Code
	MOV EBX,[ESP + 16]	;win，窗口句柄，本质为图层地址
	MOV EAX,[ESP + 20]	;x0,int,4字节
	MOV ECX,[ESP + 24]	;y0
	MOV ESI,[ESP + 28]	;x1
	MOV EDI,[ESP + 32]	;y1
	INT 0x40 			;API中断
	POP EBX				;出栈，恢复现场
	POP ESI
	POP EDI
	RET

_api_linewin:		;void api_linewin(int win,int x0,int y0,int x1,int y1, int vcolor);
	PUSH EDI
	PUSH ESI
	PUSH EBP
	PUSH EBX
	MOV EDX,13
	MOV EBX,[ESP+20]	; win
	MOV EAX,[ESP+24]	; x0
	MOV ECX,[ESP+28]	; y0
	MOV ESI,[ESP+32]	; x1
	MOV EDI,[ESP+36]	; y1
	MOV EBP,[ESP+40]	; col
	INT 0x40
	POP EBX
	POP EBP
	POP ESI
	POP EDI
	RET
	
_api_closewin:	;void api_closewin(int win);
	PUSH EBX
	MOV EDX,14
	MOV EBX,[ESP + 8]
	INT 0x40
	POP EBX
	RET

;内存相关======================================================	
_api_initmalloc:	;void api_initmalloc();
	PUSH EBX
	MOV EDX,8
	MOV EBX,[CS:0x0020]
	MOV EAX,EBX
	ADD EAX,32*1024
	MOV ECX,[CS:0x0000]
	SUB ECX,EAX
	INT 0x40
	POP EBX
	RET

_api_malloc:		;char *api_malloc(int size);
	PUSH EBX
	MOV EDX,9
	MOV EBX,[CS:0x0020]
	MOV ECX,[ESP+8]			; size
	INT 0x40
	POP EBX
	RET

_api_free:			;void api_free(char *addr, int size);
	PUSH EBX
	MOV EDX,10
	MOV EBX,[CS:0x0020]
	MOV EAX,[ESP+ 8] ; addr
	MOV ECX,[ESP+12] ; size
	INT 0x40
	POP EBX
	RET
		
;控制相关======================================================
_api_return:		;void api_return();
	MOV EDX,4		;开关4，结束应用并返回
	INT 0x40
	
;输入相关======================================================	
_api_getkey:		;int api_getkey(int mode);
	MOV EDX,15
	MOV EAX,[ESP+4]	;mode
	INT 0x40
	RET
	
;定时器相关====================================================
_api_alloctimer:					;int api_alloctimer(void);
		MOV		EDX,16
		INT		0x40
		RET

_api_inittimer:						;void api_inittimer(int timer, int data);
		PUSH	EBX
		MOV		EDX,17
		MOV		EBX,[ESP+ 8]		; timer
		MOV		EAX,[ESP+12]		; data
		INT		0x40
		POP		EBX
		RET

_api_settimer:						;void api_settimer(int timer, int time);
		PUSH	EBX
		MOV		EDX,18
		MOV		EBX,[ESP+ 8]		; timer
		MOV		EAX,[ESP+12]		; time
		INT		0x40
		POP		EBX
		RET

_api_freetimer:						;void api_freetimer(int timer);
		PUSH	EBX
		MOV		EDX,19
		MOV		EBX,[ESP+ 8]		; timer
		INT		0x40
		POP		EBX
		RET
		
;蜂鸣发声器相关====================================================
_api_beep:							;void api_beep(int tone);
		MOV		EDX,20
		MOV		EAX,[ESP+4]			; tone
		INT		0x40
		RET
		
;文件处理相关======================================================
_api_fopen:				;int api_fopen(char *fileName);
  PUSH EBX
  MOV  EDX,21
  MOV  EBX,[ESP+8]   	;fileName
  INT  0x40
  POP  EBX
  RET

_api_fclose:      		;void api_fclose(int fileHanle);
	MOV  EDX,22
	MOV  EAX,[ESP+4]   	;fileHanle
	INT  0x40
	RET

_api_fseek:				;void api_fseek(int fileHanle, int offset, int mode);
	PUSH EBX
	MOV  EDX,23
	MOV  EAX,[ESP+8]   	;fileHanle
	MOV  ECX,[ESP+16]  	;mode
	MOV  EBX,[ESP+12]  	;offset
	INT  0x40
	POP  EBX
	RET

_api_fsize:       		;int api_fsize(int fileHanle, int mode);
	MOV  EDX,24
	MOV  EAX,[ESP+4]   	;fileHanle
	MOV  ECX,[ESP+8]   	;mode
	INT  0x40
	RET

_api_fread:       		;int api_fread(char *buffer, int maxsize, int fileHanle);
	PUSH EBX
	MOV  EDX,25
	MOV  EAX,[ESP+16]	;fileHanle
	MOV  ECX,[ESP+12]  	;maxsize
	MOV  EBX,[ESP+8]   	;buffer
	INT  0x40
	POP  EBX
	RET
	
;控制台相关======================================================
_api_command:				;int api_cmdline(char *buffer, int maxSize);
	PUSH EBX
	MOV EDX,26
	MOV ECX,[ESP+12]		;maxsize
	MOV EBX,[ESP+8]			;buffer
	INT 0x40
	POP EBX
	RET

;字体相关======================================================
_api_getlang:				;int api_getlang();
	MOV EDX,27
	INT 0x40
	RET


		
	
	
	
		