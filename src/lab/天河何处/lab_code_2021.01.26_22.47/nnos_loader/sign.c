#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void die(const char *str, ...)
{
  va_list args;
  va_start(args, str);
  vfprintf(stderr, str, args);
  va_end(args);
  fputc('\n', stderr);
  exit(1);
}

int main(int argc, char *argv[])
{
  int fd;
  ssize_t size;
  char buf[512];

  if (argc != 2)
  {
    die("usage: ./sign bin_file");
  }

  if ((fd = open(argv[1], O_RDWR)) == -1)
  {
    die("open %s error", argv[1]);
  }

  if ((size = read(fd, buf, 1024)) == -1)
  {
    die("read %s error", argv[1]);
  }

  if (size > 510)
  {
    die("boot block too large: %d bytes (max 510)", size);
  }

  while ((510 - size) != 0)
  {
    buf[size++] = 0;
  }

  buf[510] = 0x55;
  buf[511] = 0xAA;

  if (lseek(fd, 0, SEEK_SET) == -1)
  {
    die("lseek error");
  }

  if ((size = write(fd, buf, 512)) != 512)
  {
    die("write %s error", argv[1]);
  }

  return 0;
}
