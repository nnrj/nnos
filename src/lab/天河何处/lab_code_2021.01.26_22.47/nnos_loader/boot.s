//#include "asm.h"

.set PROT_MODE_CSEG, 0x08        # code segment selector
.set PROT_MODE_DSEG, 0x10        # data segment selector

.code16
.globl start
start:
  cli
set_video_mode:
  xorw %ax,                %ax
  movw %ax,                %ds
  movw %ax,                %es
  movw $0x800,			   %di
check_vbe:
  movb $0x4f,			   %ah			# 表示使用VBE标准
  movb $0x00,			   %al			# 功能号
  int  $0x10
  cmp  $0x004f,			   %ax			# 若有VBE，AX应该为0x004f
  jne  set_mode_vga_0x13
  movw 0x04(%di),		   %ax
  cmp  $0x0200,			   %ax			# 若VBE版本不是2.0以上
  jb   set_mode_vga_0x13
check_vbe_mode:							# 检查MODE_VBE_0x13的参数
  movw $0x105,	           %cx
  movb $0x4f,			   %ah			# 表明VBE标准
  movb $0x01,			   %al			# 子功能号
  int  $0x10
  cmpb $0x00,			   %ah			# 是否调用成功
  jne  set_mode_vga_0x13
  cmpb $0x4f,			   %al			# 是否支持该模式
  jne  set_mode_vga_0x13
  movw (%di),              %ax
  andw $0x0080,            %ax
  jz   set_mode_vga_0x13
save_video_mode_info:					# 记录切换到的模式的一些参数信息
  movw $0x105,             video_mode
  movw 0x12(%di),		   %ax
  movw %ax,				   screen_x
  movw 0x14(%di),		   %ax
  movw %ax,				   screen_y
  movb 0x19(%di),		   %al
  movb %al,				   bits_per_pixel
  movb 0x1b(%di),		   %al
  movb %al,			       memory_model
  movl 0x28(%di),		   %eax
  movl %eax,			   video_ram
  movw $1,				   %ax
set_mode_vbe:							# 下面设置模式
  movw $0x105,	           %bx
  addw $0x4000,			   %bx			# BX第14个比特表示是否使用大的线性缓存区
  movb $0x4f,			   %ah			# 表示使用VBE标准
  movb $0x02,			   %al			# 功能号，表示设置模式
  int  $0x10
  jmp  pmode

set_mode_vga_0x13:						# 若不支持VBE则设置为VGA 0x13 mode
  movb $0,		           %ah          # 功能号0x0
  movb $0x13,	           %al          # 显示模式
  int  $0x10
  movw $320,			   screen_x
  movw $200,			   screen_y
  movl 0x28(%di),		   %eax
  movl %eax,			   video_ram
  ret
pmode:

  # Enable A20
  inb  $0x92, %al
  orb  $0x2, %al
  outb %al, $0x92

  # Load GDT
  lgdt gdtdesc
  
  # Switch from real to protected mode
  movl %cr0, %eax
  orl  $0x1, %eax
  movl %eax, %cr0

  # Jump into 32-bit protected mode
  ljmp $PROT_MODE_CSEG, $start32

.code32
start32:
  movw $PROT_MODE_DSEG, %ax
  movw %ax, %ds
  movw %ax, %es
  movw %ax, %fs
  movw %ax, %gs
  movw %ax, %ss

  movl $start, %esp
  call _bootmain     # 从这里开始运行C代码
#用绿色清屏，测试LFB地址是否正确。  
/*  push %es
  push %ds
  pusha  
  movl %ds:(video_ram),%eax
  movl %eax,%ds:(lfbbase)
  movl %ds:(lfbbase),%edi
  movl $0x0,%edx
  movw %ds:(screen_x),%cx 
.lx:
  push %cx
  mov  %ds:(screen_y),%cx
.ly: 
  movb $0x3,(%edi,%edx,1)
  inc  %edx
  loop .ly
  pop  %cx
  loop .lx
  popa   
  pop  %ds
  pop  %es
  nop
  nop
*/  

spin:
  jmp  spin

.p2align 2
gdt:
  .quad	0x0000000000000000			# 空描述符
  .quad	0x00cf9a000000ffff			# 代码段描述符
  .quad	0x00cf92000000ffff			# 数据段描述符
  .quad	0x0000000000000000			# 留待以后使用
  .quad	0x0000000000000000			# 留待以后使用

gdtdesc:
  .word gdtdesc - gdt - 1
  .long gdt
video_mode:
  .short 0xff2
screen_x:							# 水平分辨率
  .short 0xff4 
screen_y:							# 垂直分辨率
  .short 0xff6	
bits_per_pixel:
  .byte  0
memory_model:
  .byte  0
video_ram:							# video_ram地址
  .long	 0xff8
 lfbbase:
  .long  0 