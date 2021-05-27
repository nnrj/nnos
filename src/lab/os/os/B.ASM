        .486p
        org 7c00h            
code    segment  para 'code' use16
        assume   cs:code
_startup:

        mov sp,7c00h
        mov ax,0600h
        mov bx,0700h
        mov cx,0
        mov dx,184fh
        int 10h     
        mov ax,0b800h
        mov gs,ax
        mov byte ptr gs:[00h],'m'
        mov byte ptr gs:[02h],'b'
        mov byte ptr gs:[04h],'r'  
        mov eax,02h    
        mov bx,900h    
        mov cx,4
        call read_disk
        db 0e9h
        db 0c6h
        db 8ch        

read_disk:
        mov esi,eax     
        mov di,cx    
	

        mov dx,1f2h
	mov al,cl
	out dx,al
        mov eax,esi     
	

	mov cl,8
        mov dx,1f3h
	out dx,al
        
        mov dx,1f4h
	shr eax,cl
	out dx,al
        mov dx,1f5h
	shr eax,cl
	out dx,al
        
        mov dx,1f6h
	shr eax,cl
        and al,0fh    
        or al,0e0h      
	out dx,al
	

        mov dx,1f7h
        mov al,20h
	out dx,al


.not_ready:
	nop
	in al,dx
        and al,88h
        cmp al,08h
	jnz .not_ready
	

	mov ax,di
	mov dx,256
	mul dx
	mov cx,ax
	
        mov dx,1f0h
.go_on_read:
        in ax,dx
        mov [bx],ax
        add bx,2
        loop .go_on_read
        ret
	
        db 372 dup(0)
;times 510-($-$$) db 0
        db 55h,0aah
code    ends
        end    _startup
