.set PROT_MODE_CSEG, 0x08        # code segment selector
.set PROT_MODE_DSEG, 0x10        # data segment selector

.globl start
.extern _start
start:
.code16
  # close CPU interrupt
  cli
  # clear screen
  movb  $0x06,	%ah
  movb  $0x00,	%al		# roll up all rows, clear the screen
  movb  $0x00,	%ch		# row of left top corner
  movb  $0x00,	%cl		# col of left top corner
  movb  $0x18,	%dh		# row of right bottom corner
  movb  $0x4f,	%dl		# col of right bottom corner
  movb  $0x07,	%bh		# property of roll up rows
  int	$0x10
  # Enable A20
  inb   $0x92,  %al
  orb   $0x2,   %al
  outb  %al,    $0x92

  # Load GDT
  lgdt  gdt_ptr

  # Switch from real to protected mode
  movl  %cr0,   %eax
  orl   $0x1,   %eax
  movl  %eax,   %cr0

  # Jump into 32-bit protected mode
  ljmpl $0x08,  $protcseg

.code32
protcseg:
  movw  $0x10,  %ax
  movw  %ax,    %ds
  movw  %ax,    %es
  movw  %ax,    %fs
  movw  %ax,    %gs
  movw  %ax,    %ss
  movl  $_start, %esp
  
  ljmpl $0x08,  $enter_kernel

enter_kernel:
  # 直接解析二进制内核文件,解析ELF文件的代码在kernel_init  
  # 挪动C内核到0x100000处并从这里开始运行C内核
  movl  $512*1024/4,%ecx
  pushl %ecx
  # 先压入第三个参数要拷贝的内存的大小
  movl  $_start,%esi
  pushl %esi
  # 再压入第二个参数源地址
  movl  $0x100000,%edi
  pushl %edi
  # 最后压入第一个参数目的地址顺序是从右往左压入  
  call  mem_cpy  
  # 由调用者清理栈中的参数
  addl  $12,%esp
  movl  $_start,%ebx  
  movl  %ebx,%esp   
  
  call  _start
  
   
mem_cpy:
  cld    
  pushl %ebp
  movl  %esp,%ebp
  pushl %esi
  pushl %edi
  pushl %ecx
  movl  0x8(%ebp),  %edi
  movl  0xc(%ebp),  %esi
  movl  0x10(%ebp), %ecx
  shr   $2,%ecx  
  rep   movsw
  movl  0x10(%ebp),%ecx
  andl  $3,%ecx
  rep   movsb  
  popl  %ecx
  popl  %edi
  popl  %esi
  popl  %ebp
  ret   	
  
spin:
  jmp   spin  
 
 .p2align 2
  # gdt表
gdt:
  .quad 0x0000000000000000 # 空描述符
  .quad 0x00cf98000000ffff # 内核的CODE段描述符
  .quad 0x00cf92000000ffff # 内核的DATA段描述符
  .quad 0x00cffa000000ffff # 用户的CODE段描述符
  .quad 0x00cff2000000ffff # 用户的DATA段描述符

gdt_ptr:
  .word gdt_ptr - gdt - 1
  .long gdt 
_start: 
  
 
  
  