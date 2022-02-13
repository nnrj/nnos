#include "elf.h"
#include "naskfunc.h"

#define SECTSIZE 512

void readseg(unsigned char *pa, unsigned int count, unsigned int offset);

void bootmain(void)
{
  struct elfhdr *elf;
  struct proghdr *ph, *eph;
  void (*entry)(void);
  unsigned char *pa;

  elf = (struct elfhdr *)0x10000;

  readseg((unsigned char *)elf, 4096, 0);

  if (elf->magic != ELF_MAGIC)
    return;

  ph = (struct proghdr *)((unsigned char *)elf + elf->phoff);
  eph = ph + elf->phnum;
  for (; ph < eph; ph++)
  {
    pa = (unsigned char *)ph->paddr;
    readseg(pa, ph->filesz, ph->off);
    for (int i = 0; i < ph->memsz - ph->filesz; i++)
    {
      *((char *)ph->paddr + ph->filesz + i) = 0;
    }
  }

  entry = (void (*)(void))(elf->entry);
  entry();
}

void waitdisk(void)
{
  while ((inb(0x1F7) & 0xC0) != 0x40)
    ;
}

void readsect(void *dst, unsigned int offset)
{
  waitdisk();
  outb(0x1F2, 1);
  outb(0x1F3, offset);
  outb(0x1F4, offset >> 8);
  outb(0x1F5, offset >> 16);
  outb(0x1F6, (offset >> 24) | 0xE0);
  outb(0x1F7, 0x20);

  waitdisk();
  insl(0x1F0, dst, SECTSIZE / 4);
}

void readseg(unsigned char *pa, unsigned int count, unsigned int offset)
{
  unsigned char *epa;
  epa = pa + count;

  pa -= offset % SECTSIZE;
  offset = (offset / SECTSIZE) + 8287; //此处所加的数字等于elf文件在FAT32文件系统里的物理扇区号

  for (; pa < epa; pa += SECTSIZE, offset++)
    readsect(pa, offset);
}
