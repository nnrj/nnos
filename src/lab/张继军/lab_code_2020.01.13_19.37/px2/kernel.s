#boot.s -- zenglOX kernel start assemble
.equ MBOOT_PAGE_ALIGN, 1<<0 #Load Kernel on a page boundary
.equ MBOOT_GETMEM_INFO,1<<1 #Tell MBoot provide your kernel with memory info
.equ MBOOT_MAGIC,0x1BADB002 #Multiboot Magic value

.equ MBOOT_FLAGS,MBOOT_PAGE_ALIGN | MBOOT_GETMEM_INFO
.equ MBOOT_CHECKSUM,-(MBOOT_MAGIC + MBOOT_FLAGS)

.align 4
.global _mboot
.extern _code
.extern _bss
.extern _end
_mboot:
  .long MBOOT_MAGIC
  .long MBOOT_FLAGS
  .long MBOOT_CHECKSUM

  .long _mboot
  .long _code
  .long _bss
  .long _end
  .long _start

.section .text
.global _start
.extern _kernel_main
_start:
  
  pushl %ebx
  cli
  call _kernel_main
  #movb  $'p',   %ds:0xb800e
  #movb  $0x0c,  %ds:0xb800f
  hlt
boot_hang:
  jmp boot_hang

