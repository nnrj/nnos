// main.c -- Defines the C-code kernel entry point, calls initialisation routines.
//           Made for JamesM's tutorials <www.jamesmolloy.co.uk>

#include "monitor.h"
#define PTE_P 0x1
#define PTE_W 0x2
__attribute__((__aligned__(4096)))
unsigned int pgdir[1024];

__attribute__((__aligned__(4096)))
unsigned int pt[1024];


void kernel_main(void)
{
    // Initialise the screen (by clearing it)
    monitor_clear();
    // Write out a sample string
	pgdir[0] = (unsigned int)pt | PTE_P | PTE_W;

    for (int i = 0; i < 1024; i++)
     {
     pt[i] = (i << 12) | PTE_P | PTE_W;
     }

    asm volatile("movl %0, %%cr3" ::"r"(pgdir));

    unsigned int cr0;
    asm volatile("movl %%cr0, %0"
              : "=r"(cr0));
    cr0 |= 0x80000000;
    asm volatile("movl %0, %%cr0" ::"r"(cr0));
    monitor_write("Hello, paging world!\nNnos ver: 0.0.1");
	
	
    
    return ;
}
