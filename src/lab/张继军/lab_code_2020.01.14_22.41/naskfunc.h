/*
 * zhangjijun
 * 2022-01-14
 */

#ifndef _NASKFUNC_H_
#define _NASKFUNC_H_
static inline void io_hlt()
{
  __asm__ __volatile__ ( 
  "  hlt\n"
  "  ret\n"
  :
  :
  :"memory", "esi", "edi", "eax", "ebx", "ecx", "edx");
}
static inline void io_cli()
{ 
  __asm__ __volatile__ (
  "  cli\n"
  "  ret\n"
  :
  :
  :"memory", "esi", "edi", "eax", "ebx", "ecx", "edx");
}
static inline void io_sti()
{
	__asm__ __volatile__ (
  "  sti\n"
  "  ret\n"
  :
  :
  :"memory", "esi", "edi", "eax", "ebx", "ecx", "edx");
}	
static inline unsigned char inb(unsigned short port)
{
    unsigned short data;
    __asm__ volatile("inb %1,%0" : "=a" (data) : "d" (port));
    return data;
}

static inline unsigned char inw(unsigned short port)
{
    unsigned short data;
    __asm__ volatile("inw %1,%0" : "=a" (data) : "d" (port));
    return data;
}

static inline void insl(int port, void *addr, int cnt)
{
    __asm__ volatile("cld; rep insl" :
            "=D" (addr), "=c" (cnt) :
            "d" (port), "0" (addr), "1" (cnt) :
            "memory", "cc");
}

static inline void outb(unsigned short port, unsigned char data)
{
    __asm__ volatile("outb %0,%1" : : "a" (data), "d" (port));
}

static inline void outw(unsigned short port, unsigned short data)
{
    __asm__ volatile("outw %0,%1" : : "a" (data), "d" (port));
}

static inline void outl(unsigned short port, unsigned int val)
{
	asm volatile("outl %0,%1" : : "a" (val), "dN" (port));
}

static inline unsigned int inl(unsigned short port)
{
	unsigned int v;
	asm volatile("inl %1,%0" : "=a" (v) : "dN" (port));
	return v;
}

static inline void outsl(int port, const void *addr, int cnt)
{
    __asm__ volatile("cld; rep outsl" :
            "=S" (addr), "=c" (cnt) :
            "d" (port), "0" (addr), "1" (cnt) :
            "cc");
}

static inline void stosb(void *addr, int data, int cnt)
{
    __asm__ volatile("cld; rep stosb" :
            "=D" (addr), "=c" (cnt) :
            "0" (addr), "1" (cnt), "a" (data) :    /* di=addr, cx=cnt, ax=data */
            "memory", "cc");
}

static inline void movsb(void *dst, void *src, int cnt)
{
    __asm__ volatile("cld; rep movsb" :
            :
            "D" (dst), "S" (src), "c" (cnt) :      /* di=dst, si=src, cx=cnt */
            "memory", "cc");
}

static inline unsigned int get_cr0(void)
{
    unsigned int val;
    __asm__ volatile("movl %%cr0, %0" : "=r" (val));
    return val;
}

static inline void set_cr0(unsigned int val)
{
    __asm__ volatile("movl %0, %%cr0" : : "r" (val));
}

static inline void set_cr3(unsigned int val)
{
    __asm__ volatile("movl %0, %%cr3" : : "r" (val));
}

#define CMOS_ADDR_PORT		0x70
#define CMOS_DATA_PORT		0x71

static inline unsigned int cmos_read(unsigned int reg)
{
    outb(CMOS_ADDR_PORT, reg);
    return inb(CMOS_DATA_PORT);
}

static inline void cmos_write(unsigned int reg, unsigned int value)
{
    outb(CMOS_ADDR_PORT, reg);
    outb(CMOS_DATA_PORT, value);
}

static inline unsigned int nvram_read(unsigned int reg)
{
    return cmos_read(reg) | cmos_read(reg+1) << 8;
}

static inline void lgdt(void* gdt, unsigned int size)
{
    volatile unsigned short pd[3];

    pd[0] = size-1;
    pd[1] = (unsigned int)gdt;
    pd[2] = (unsigned int)gdt >> 16;

    __asm__ volatile("lgdt (%0)" : : "r" (pd));
}

static inline void lidt(void* idt, unsigned int size)
{
    volatile unsigned short pd[3];

    pd[0] = size-1;
    pd[1] = (unsigned int)idt;
    pd[2] = (unsigned int)idt >> 16;

    __asm__ volatile("lidt (%0)" : : "r" (pd));
}

static inline void ltr(unsigned short sel)
{
    __asm__ volatile("ltr %0" : : "r" (sel));
}

static inline void sti(void)
{
    __asm__ volatile("sti");
}

static inline void cli(void)
{
    __asm__ volatile("cli");
}

static inline void halt(void)
{
    __asm__ volatile("hlt");
}

static inline unsigned int xchg(volatile unsigned int *addr, unsigned int newval)
{
    unsigned int result;

    // "+m": read-modify-write operand
    __asm__ volatile("lock; xchgl %0, %1" :
            "+m" (*addr), "=a" (result) :
            "1" (newval) :
            "cc");
    return result;
}

static inline int change_bit(int nr, void* addr)
{
    int oldbit;

    __asm__ __volatile__("btcl %2,%1\n\tsbbl %0,%0"
            :"=r" (oldbit),"=m" (*((unsigned *)(addr)))
            :"r" (nr));
    return oldbit;
}

#define rdtsc(low,high) \
     __asm__ __volatile__("rdtsc" : "=a" (low), "=d" (high))

#define rdtsc64(val) \
     __asm__ __volatile__("rdtsc" : "=A" (val))

#define rdmsr(msr,val1,val2) \
	__asm__ __volatile__("rdmsr" \
			  : "=a" (val1), "=d" (val2) \
			  : "c" (msr))

#define wrmsr(msr,val1,val2) \
	__asm__ __volatile__("wrmsr" \
			  : /* no outputs */ \
			  : "c" (msr), "a" (val1), "d" (val2))


#endif

static inline void nop(void)
{
    __asm__ __volatile__("rep; nop");
}

#define mb() 	__asm__ __volatile__ ("lock; addl $0,0(%%esp)": : :"memory")
#define rmb()	mb()
#define wmb()	__asm__ __volatile__ ("": : :"memory")

#define barrier() __asm__ __volatile__("": : :"memory")

