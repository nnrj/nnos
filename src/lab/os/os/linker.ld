ENTRY(main)

SECTIONS
{
    . = 0x007c00;
    
    .text :
    {
        mbr.o(.text)
	. += 0x000200;
	boot.o(.text)
	main.o(.text)
    }
    .data :
    {
	*.o(.data)
    }
}