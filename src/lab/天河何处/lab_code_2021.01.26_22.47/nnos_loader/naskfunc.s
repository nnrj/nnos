# naskfunc
# tab=4
.intel_syntax
.code32						      # 制作32位模式用的机器语言
		.globl  _asmfunc,_mem_cpy,_getcursor,_setcursor
		.globl	_io_hlt, _io_cli, _io_sti, _io_stihlt
		.globl	_io_in8,  _io_in16,  _io_in32
		.globl	_io_out8, _io_out16, _io_out32
		.globl	_io_load_eflags, _io_store_eflags
		.globl	_load_gdtr, _load_idtr
		.globl	_load_cr0, _store_cr0
        .globl  _asm_inthandler2e, _asm_inthandler21
		.globl	_asm_inthandler27, _asm_inthandler2c
		.globl	_memtest_sub
		.globl  _read_sectors_chs,_read_sectors_lba24
        //.extern _inthandler2e, _inthandler21
		//.extern	_inthandler27, _inthandler2c
        //.extern _printstr,_printchar
.section .text
/*_asmfunc:	                       # void _asmfunc(int addr, char data,int color)#
	    push    ebp
        mov     ebp,esp            #由于push ebp导致栈顶又多挪动了4位
        push    dword ptr [ebp+16+4]   #corlor
        push    dword ptr [ebp+12+4]   #start_col_y
        push    dword ptr [ebp+ 8+4]   #start_line_x
        push    dword ptr [ebp+ 4+4]   #*s

#汇编语言调用C语言测试
#void printstr(char *s,char start_line_x,char start_col_y,char corlor)
        call    _printstr
        pop     eax                #全是为了保持函数调用时push的堆栈平衡
  		pop     eax
        pop     eax
 		pop     eax
        pop     ebp
        ret
*/		
__mem_copy:                        #void mem_copy(dst,src,size)# 
        push    ebp
        mov     ebp, esp 
        push	esi   
        push	edi 
        push	ecx       
        mov     edi,  [ebp+8]		#datadst
        mov     esi,  [ebp+12]      #datasrc
        mov     ecx,  [ebp+16]      #datalen
         
        cmp     ecx,  0             #假如长度是0则不复制任何数据
        je      nocopy
cgoon:      
		mov		eax,[esi]
		add		esi,4
		mov		[edi],eax
		add		edi,4
		loop	cgoon	
nocopy:
        pop	    ecx
        pop	    edi
        pop	    esi	
	    pop     ebp
		ret
#unsigned short getcursor(void)
_getcursor:      #########  获取当前光标位置,通过ax返回 #########
   #先取得高8位
        mov     dx, 0x03d4  #索引寄存器
        mov     al, 0x0e	#用于提供光标位置的高8位
        out     dx, al
        mov     dx, 0x03d5  #通过读写数据端口0x3d5来获得或设置光标位置
        in      al, dx	    #得到了光?位置的高8位
        mov     ah, al
   #再取取得低8位
        mov     dx, 0x03d4
        mov     al, 0x0f
        out     dx, al
        mov     dx, 0x03d5 
        in      al, dx
        ret              
                              
#void setcursor(unsigned short  pos_cursor)                
_setcursor:  #通过bx设置新的光标
        push    ebp
        mov     ebp,esp                
 		#由于push ebp导致栈顶又多挪动了4位
        mov     ebx,dword ptr [ebp+04+4]  # 光?位置参数 
####### 1 先设置高8位 ########
        mov     dx, 0x03d4			  #索引寄存器
        mov     al, 0x0e			  #用于提供光标位置的高8位
        out     dx, al
        mov     dx, 0x03d5			  #通过读写数据端口0x3d5来获得或设置光标位置 
        mov     al, bh
        out     dx, al

####### 2 再设置低8位 #########
        mov     dx, 0x03d4
        mov     al, 0x0f
        out     dx, al
        mov     dx, 0x03d5 
        mov     al, bl
        out     dx, al
   
        pop     ebp
        ret 
_io_hlt:	# void io_hlt(void)#
		hlt
		ret

_io_cli:	# void io_cli(void)#
		cli
		ret

_io_sti:	# void io_sti(void)#
		sti
		ret

_io_stihlt:	# void io_stihlt(void)#
		sti
		hlt
		ret

_io_in8:	# int io_in8(int port)#
		mov		edx,[esp+4]		# port
		mov		eax,0
		in		al,dx
		ret

_io_in16:	# int io_in16(int port)#
		mov		edx,[esp+4]		# port
		mov		eax,0
		in		ax,dx
		ret

_io_in32:	# int io_in32(int port)#
		mov		edx,[esp+4]		# port
		in		eax,dx
		ret

_io_out8:	# void io_out8(int port, int data)#
		mov		edx,[esp+4]		# port
		mov		al,[esp+8]		# data
		out		dx,al
		ret

_io_out16:	# void io_out16(int port, int data)#
		mov		edx,[esp+4]		# port
		mov		eax,[esp+8]		# data
		out		dx,ax
		ret

_io_out32:	# void io_out32(int port, int data)#
		mov		edx,[esp+4]		# port
		mov		eax,[esp+8]		# data
		out		dx,eax
		ret

_io_load_eflags:	# int io_load_eflags(void)#
		pushfd		# push eflags 
		pop		eax
		ret

_io_store_eflags:	# void io_store_eflags(int eflags)#
		mov		eax,[esp+4]
		push	eax
		popfd		# pop eflags 
		ret

_load_gdtr:		# void load_gdtr(int limit, int addr)#
		mov		ax,[esp+4]		# limit
		mov		[esp+6],ax
		lgdt	[esp+6]
		ret

_load_idtr:		# void load_idtr(int limit, int addr)#
		mov		ax,[esp+4]		# limit
		mov		[esp+6],ax
		lidt	[esp+6]
		ret

_load_cr0:		# int load_cr0(void)#
		mov		eax,cr0
		ret

_store_cr0:		# void store_cr0(int cr0)#
		mov		eax,[esp+4]
		mov		cr0,eax
		ret

/*_asm_inthandler2e:
		push	es
		push	ds
		pushad
		mov		eax,esp
		push	eax
		mov		ax,ss
		mov		ds,ax
		mov		es,ax
        call    _inthandler2e
		pop		eax
		popad
		pop		ds
		pop		es
		iretd

_asm_inthandler21:
		push	es
		push	ds
		pushad
		mov		eax,esp
		push	eax
		mov		ax,ss
		mov		ds,ax
		mov		es,ax
		call	_inthandler21
		pop		eax
		popad
		pop		ds
		pop		es
		iretd

_asm_inthandler27:
		push	es
		push	ds
		pushad
		mov		eax,esp
		push	eax
		mov		ax,ss
		mov		ds,ax
		mov		es,ax
		call	_inthandler27
		pop		eax
		popad
		pop		ds
		pop		es
		iretd

_asm_inthandler2c:
		push	es
		push	ds
		pushad
		mov		eax,esp
		push	eax
		mov		ax,ss
		mov		ds,ax
		mov		es,ax
		call	_inthandler2c
		pop		eax
		popad
		pop		ds
		pop		es
		iretd
*/
_memtest_sub:	# unsigned int memtest_sub(unsigned int start, unsigned int end)
		push	edi						# （由于还要使用ebx, esi, edi）
		push	esi
		push	ebx
		mov		esi,0xaa55aa55			# pat0 = 0xaa55aa55#
		mov		edi,0x55aa55aa			# pat1 = 0x55aa55aa#
		mov		eax,[esp+12+4]			# i = start#
mts_loop:
		mov		ebx,eax
		add		ebx,0xffc				# p = i + 0xffc#
		mov		edx,[ebx]				# old = *p#
		mov		[ebx],esi				# *p = pat0#
		xor		dword ptr [ebx],0xffffffff	# *p ^= 0xffffffff#
		cmp		edi,[ebx]				# if (*p != pat1) goto fin#
		jne		mts_fin
		xor		dword ptr [ebx],0xffffffff	# *p ^= 0xffffffff#
		cmp		esi,[ebx]				# if (*p != pat0) goto fin#
		jne		mts_fin
		mov		[ebx],edx				# *p = old#
		add		eax,0x1000				# i += 0x1000#
		cmp		eax,[esp+12+8]			# if (i <= end) goto mts_loop#
		jbe		mts_loop
		pop		ebx
		pop		esi
		pop		edi
		ret
mts_fin:
		mov		[ebx],edx				# *p = old#
		pop		ebx
		pop		esi
		pop		edi
		ret
#--------------------------------------		
check_ready:
        mov     dx,0x01f7
        in      al,dx
        test    al,0x40
        jz      check_ready
        ret
check_read_complete:
        mov     dx,0x01f7
        in      al,dx
        test    al,0x08
        jz      check_ready
        ret
pio_delay:
        nop
        nop
        nop
        nop
        ret
#--------------------------------------	
#read_sectors(int sector_count,int sector_number,int cyl,int driver,int head,int buffer)
_read_sectors_chs:  
        push    ebp
        mov     ebp,esp
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    edi
        call    check_ready
#--------------------------------------
        mov     dx,0x01f2
        mov     eax,dword ptr [ebp+8]
        #mov     al,1
        out     dx,al
        call    pio_delay
#--------------------------------------
        mov     dx,0x01f3
        mov     eax,dword ptr [ebp+12]
        #mov     al,1
        out     dx,al
        call    pio_delay
#--------------------------------------
        mov     ebx,dword ptr [ebp+16]
        and     bx,0x03ff
        mov     dx,0x01f4
        mov     al,bl
        out     dx,al
        call    pio_delay
#--------------------------------------
        mov     dx,0x01f5
        mov     al,bh
        out     dx,al
        call    pio_delay
#--------------------------------------
        mov     ebx,dword ptr [ebp+20]
        and     ebx,1
        shl     bl,4
        mov     al,0x0a0
        or      al,bl
        mov     ebx,dword ptr [ebp+24]
        and     bl,0x0f
        or      al,bl
        mov     dx,0x01f6
        out     dx,al
        mov     dx,0x01f7
        mov     al,0x20
        out     dx,al
        call    check_read_complete
#--------------------------------------
        mov     eax,256
        mov     ebx,dword ptr [ebp+8]
		mul     bx
        mov     cx,ax
        mov     edi,dword ptr [ebp+28]
        mov     dx,0x01f0
        rep     insw
#--------------------------------------		
        pop     edi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        pop     ebp
        xor     eax,eax
        ret
#--------------------------------------
#read_sectors_lba24(int sector_number,int sector_count,int driver,int buffer)		
_read_sectors_lba24:
        push    ebp
        mov     ebp,esp
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    edi
        call    check_ready
#--------------------------------------		
        mov     dx,0x01f2
	    mov     eax,dword ptr [ebp+12]
        out     dx,al
        call    pio_delay
#--------------------------------------		
        mov     dx,0x01f3
        mov     eax,dword ptr [ebp+8]
        out     dx,al
        call    pio_delay
#--------------------------------------		
        mov     dx,0x01f4
        mov     eax,dword ptr [ebp+8]
        shr     eax,8
        out     dx,al
        call    pio_delay
#--------------------------------------		
        mov     dx,0x01f5
        mov     eax,dword ptr [ebp+8]
        shr     eax,16
        out     dx,al
        call    pio_delay
#--------------------------------------		
        mov     ebx,dword ptr [ebp+16]
        and     ebx,1
        shl     bl,4
        mov     al,0x0e0
        or      al,bl
		mov     ebx,dword ptr [ebp+8]
		shr     ebx,24
		and     bl,0x0f
		or      al,bl
        mov     dx,0x01f6
        out     dx,al
        mov     dx,0x01f7
        mov     al,0x20
        out     dx,al
        call    check_read_complete
#--------------------------------------		
        mov     eax,256
        mov     ebx,dword ptr [ebp+12]
        mul     bx
        mov     cx,ax
        mov     edi,dword ptr [ebp+20]
        mov     dx,0x01f0
        rep     insw
#--------------------------------------		
        pop     edi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        pop     ebp
        xor     eax,eax
        ret
