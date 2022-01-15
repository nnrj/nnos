; naskfunc
; TAB=4
[BITS 32]						; 3制作32位模式用的机器语言
		GLOBAL  _asmfunc,_mem_cpy,_getcursor,_setcursor
		GLOBAL	_io_hlt, _io_cli, _io_sti, _io_stihlt
		GLOBAL	_io_in8,  _io_in16,  _io_in32
		GLOBAL	_io_out8, _io_out16, _io_out32
		GLOBAL	_io_load_eflags, _io_store_eflags
		GLOBAL	_load_gdtr, _load_idtr
		GLOBAL	_load_cr0, _store_cr0
        GLOBAL  _asm_inthandler2E, _asm_inthandler21
		GLOBAL	_asm_inthandler27, _asm_inthandler2c
		GLOBAL	_memtest_sub
		GLOBAL  _READ_SECTORS_CHS,_READ_SECTORS_LBA24
        EXTERN  _inthandler2E, _inthandler21
		EXTERN	_inthandler27, _inthandler2c
        EXTERN  _printstr,_printchar
[SECTION .text]
_asmfunc:	                       ; void _asmfunc(int addr, char data,int color);
	    push    ebp
        mov     ebp,esp            ;由于push ebp导致栈顶又多挪动了4位
        push    dword [EBP+16+4]   ;corlor
        push    dword [EBP+12+4]   ;start_col_y
        push    dword [EBP+08+4]   ;start_line_x
        push    dword [EBP+04+4]   ;*s

;汇编语言调用C语言测试
;void printstr(char *s,char start_line_x,char start_col_y,char corlor)
        CALL    _printstr
        pop     eax                ;全是为了保持函数调用时push的栈平衡
  		pop     eax
        pop     eax
 		pop     eax
        pop     ebp
        ret
__mem_copy:                        ;void mem_copy(dst,src,size); 
        push    ebp
        mov     ebp, esp 
        PUSH	ESI   
        PUSH	EDI 
        PUSH	ECX       
        mov     edi,  [ebp+8]		;datadst
        mov     esi,  [ebp+12]      ;datasrc
        mov     ecx,  [ebp+16]      ;datalen
         
        cmp     ecx,  0             ;假如长度是0则不复制任何数据
        je      nocopy
cgoon:      
		MOV		EAX,[ESI]
		ADD		ESI,4
		MOV		[EDI],EAX
		ADD		EDI,4
		loop	cgoon	
nocopy:
        POP	    ECX
        POP	    EDI
        POP	    ESI	
	    pop     ebp
		RET
;unsigned short getcursor(void)
_getcursor:      ;;;;;;;;;  获取当前光标位置,通过ax返回 ;;;;;;;;;
   ;先获得高8位
        mov     dx, 0x03d4  ;索引寄存器
        mov     al, 0x0e	   ;用于提供光标位置的高8位
        out     dx, al
        mov     dx, 0x03d5  ;通过读写数据端口0x3d5来获得或设置光标位置 
        in      al, dx	   ;得到了光标位置的高8位
        mov     ah, al
   ;再获取低8位
        mov     dx, 0x03d4
        mov     al, 0x0f
        out     dx, al
        mov     dx, 0x03d5 
        in      al, dx
        ret              
                              
;void setcursor(unsigned short  pos_cursor)                
_setcursor:  ;通过BX设置新的光标值 
        push    ebp
        mov     ebp,esp                
 		;由于push ebp导致栈顶又多挪动了4位
        mov     ebx,dword [EBP+04+4]  ; 光标位置参数 
;;;;;;; 1 先设置高8位 ;;;;;;;;
        mov     dx, 0x03d4			  ;索引寄存器
        mov     al, 0x0e				  ;用于提供光标位置的高8位
        out     dx, al
        mov     dx, 0x03d5			  ;通过读写数据端口0x3d5来获得或设置光标位置 
        mov     al, bh
        out     dx, al

;;;;;;; 2 再设置低8位 ;;;;;;;;;
        mov     dx, 0x03d4
        mov     al, 0x0f
        out     dx, al
        mov     dx, 0x03d5 
        mov     al, bl
        out     dx, al
   
        pop     ebp
        ret 
_io_hlt:	; void io_hlt(void);
		HLT
		RET

_io_cli:	; void io_cli(void);
		CLI
		RET

_io_sti:	; void io_sti(void);
		STI
		RET

_io_stihlt:	; void io_stihlt(void);
		STI
		HLT
		RET

_io_in8:	; int io_in8(int port);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,0
		IN		AL,DX
		RET

_io_in16:	; int io_in16(int port);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,0
		IN		AX,DX
		RET

_io_in32:	; int io_in32(int port);
		MOV		EDX,[ESP+4]		; port
		IN		EAX,DX
		RET

_io_out8:	; void io_out8(int port, int data);
		MOV		EDX,[ESP+4]		; port
		MOV		AL,[ESP+8]		; data
		OUT		DX,AL
		RET

_io_out16:	; void io_out16(int port, int data);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,[ESP+8]		; data
		OUT		DX,AX
		RET

_io_out32:	; void io_out32(int port, int data);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,[ESP+8]		; data
		OUT		DX,EAX
		RET

_io_load_eflags:	; int io_load_eflags(void);
		PUSHFD		; PUSH EFLAGS 
		POP		EAX
		RET

_io_store_eflags:	; void io_store_eflags(int eflags);
		MOV		EAX,[ESP+4]
		PUSH	EAX
		POPFD		; POP EFLAGS 
		RET

_load_gdtr:		; void load_gdtr(int limit, int addr);
		MOV		AX,[ESP+4]		; limit
		MOV		[ESP+6],AX
		LGDT	[ESP+6]
		RET

_load_idtr:		; void load_idtr(int limit, int addr);
		MOV		AX,[ESP+4]		; limit
		MOV		[ESP+6],AX
		LIDT	[ESP+6]
		RET

_load_cr0:		; int load_cr0(void);
		MOV		EAX,CR0
		RET

_store_cr0:		; void store_cr0(int cr0);
		MOV		EAX,[ESP+4]
		MOV		CR0,EAX
		RET

_asm_inthandler2E:
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
        CALL    _inthandler2E
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD

_asm_inthandler21:
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler21
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD

_asm_inthandler27:
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

_asm_inthandler2c:
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

_memtest_sub:	; unsigned int memtest_sub(unsigned int start, unsigned int end)
		PUSH	EDI						; （由于还要使用EBX, ESI, EDI）
		PUSH	ESI
		PUSH	EBX
		MOV		ESI,0xaa55aa55			; pat0 = 0xaa55aa55;
		MOV		EDI,0x55aa55aa			; pat1 = 0x55aa55aa;
		MOV		EAX,[ESP+12+4]			; i = start;
mts_loop:
		MOV		EBX,EAX
		ADD		EBX,0xffc				; p = i + 0xffc;
		MOV		EDX,[EBX]				; old = *p;
		MOV		[EBX],ESI				; *p = pat0;
		XOR		DWORD [EBX],0xffffffff	; *p ^= 0xffffffff;
		CMP		EDI,[EBX]				; if (*p != pat1) goto fin;
		JNE		mts_fin
		XOR		DWORD [EBX],0xffffffff	; *p ^= 0xffffffff;
		CMP		ESI,[EBX]				; if (*p != pat0) goto fin;
		JNE		mts_fin
		MOV		[EBX],EDX				; *p = old;
		ADD		EAX,0x1000				; i += 0x1000;
		CMP		EAX,[ESP+12+8]			; if (i <= end) goto mts_loop;
		JBE		mts_loop
		POP		EBX
		POP		ESI
		POP		EDI
		RET
mts_fin:
		MOV		[EBX],EDX				; *p = old;
		POP		EBX
		POP		ESI
		POP		EDI
		RET
;--------------------------------------		
CHECK_READY:
        MOV     DX,01F7H
        IN      AL,DX
        TEST    AL,40H
        JZ      CHECK_READY
        RET
CHECK_READ_COMPLETE:
        MOV     DX,01F7H
        IN      AL,DX
        TEST    AL,08H
        JZ      CHECK_READY
        RET
PIO_DELAY:
        NOP
        NOP
        NOP
        NOP
        RET
;--------------------------------------	
;read_sectors(int sector_count,int sector_number,int cyl,int driver,int head,int buffer)
_READ_SECTORS_CHS:  
        PUSH    EBP
        MOV     EBP,ESP
        PUSH    EAX
        PUSH    EBX
        PUSH    ECX
        PUSH    EDX
        PUSH    EDI
        CALL    CHECK_READY
;--------------------------------------
        MOV     DX,01F2H
        MOV     EAX,DWORD [EBP+8]
        ;MOV     AL,1
        OUT     DX,AL
        CALL    PIO_DELAY
;--------------------------------------
        MOV     DX,01F3H
        MOV     EAX,DWORD [EBP+12]
        ;MOV     AL,1
        OUT     DX,AL
        CALL    PIO_DELAY
;--------------------------------------
        MOV     EBX,DWORD [EBP+16]
        AND     BX,03FFH
        MOV     DX,01F4H
        MOV     AL,BL
        OUT     DX,AL
        CALL    PIO_DELAY
;--------------------------------------
        MOV     DX,01F5H
        MOV     AL,BH
        OUT     DX,AL
        CALL    PIO_DELAY
;--------------------------------------
        MOV     EBX,DWORD [EBP+20]
        AND     EBX,1
        SHL     BL,4
        MOV     AL,0A0H
        OR      AL,BL
        MOV     EBX,DWORD [EBP+24]
        AND     BL,0FH
        OR      AL,BL
        MOV     DX,01F6H
        OUT     DX,AL
        MOV     DX,01F7H
        MOV     AL,20H
        OUT     DX,AL
        CALL    CHECK_READ_COMPLETE
;--------------------------------------
        MOV     EAX,256
        MOV     EBX,DWORD [EBP+8]
        MUL     BX
        MOV     CX,AX
        MOV     EDI,DWORD [EBP+28]
        MOV     DX,01F0H
        REP     INSW
;--------------------------------------		
        POP     EDI
        POP     EDX
        POP     ECX
        POP     EBX
        POP     EAX
        POP     EBP
        XOR     EAX,EAX
        RET
;--------------------------------------
;read_sectors_lba24(int sector_number,int sector_count,int driver,int buffer)		
_READ_SECTORS_LBA24:
        PUSH    EBP
        MOV     EBP,ESP
        PUSH    EAX
        PUSH    EBX
        PUSH    ECX
        PUSH    EDX
        PUSH    EDI
        CALL    CHECK_READY
;--------------------------------------		
        MOV     DX,01F2H
        MOV     EAX,DWORD [EBP+12]
        OUT     DX,AL
        CALL    PIO_DELAY
;--------------------------------------		
        MOV     DX,01F3H
        MOV     EAX,DWORD [EBP+8]
        OUT     DX,AL
        CALL    PIO_DELAY
;--------------------------------------		
        MOV     DX,01F4H
        MOV     EAX,DWORD [EBP+8]
        SHR     EAX,8
        OUT     DX,AL
        CALL    PIO_DELAY
;--------------------------------------		
        MOV     DX,01F5H
        MOV     EAX,DWORD [EBP+8]
        SHR     EAX,16
        OUT     DX,AL
        CALL    PIO_DELAY
;--------------------------------------		
        MOV     EBX,DWORD [EBP+16]
        AND     EBX,1
        SHL     BL,4
        MOV     AL,0E0H
        OR      AL,BL
		MOV     EBX,DWORD [EBP+8]
		SHR     EBX,24
		AND     BL,0FH
		OR      AL,BL
        MOV     DX,01F6H
        OUT     DX,AL
        MOV     DX,01F7H
        MOV     AL,20H
        OUT     DX,AL
        CALL    CHECK_READ_COMPLETE
;--------------------------------------		
        MOV     EAX,256
        MOV     EBX,DWORD [EBP+12]
        MUL     BX
        MOV     CX,AX
        MOV     EDI,DWORD [EBP+20]
        MOV     DX,01F0H
        REP     INSW
;--------------------------------------		
        POP     EDI
        POP     EDX
        POP     ECX
        POP     EBX
        POP     EAX
        POP     EBP
        XOR     EAX,EAX
        RET
