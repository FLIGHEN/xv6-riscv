#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int
parse_uint(char *s)
{
  int n = 0;

  if(*s == 0)
    return -1;

  while(*s){
    if(*s < '0' || *s > '9')
      return -1;
    n = n * 10 + (*s - '0');
    s++;
  }

  return n;
}

int
main(int argc, char *argv[])
{
  int fd;
  int need;
  int total;
  int first;
  char buf[64];
  char *hex = "0123456789ABCDEF";

  if(argc != 3){
    fprintf(2, "usage: hexdump <count> <file>\n");
    exit(1);
  }

  need = parse_uint(argv[1]);
  if(need < 0){
    fprintf(2, "hexdump: invalid count\n");
    exit(1);
  }

  fd = open(argv[2], O_RDONLY);
  if(fd < 0){
    fprintf(2, "hexdump: cannot open %s\n", argv[2]);
    exit(1);
  }

  total = 0;
  first = 1;

  while(total < need){
    int chunk = need - total;
    if(chunk > sizeof(buf))
      chunk = sizeof(buf);

    int r = read(fd, buf, chunk);
    if(r < 0){
      fprintf(2, "hexdump: read error\n");
      close(fd);
      exit(1);
    }
    if(r == 0)
      break;

    for(int i = 0; i < r; i++){
      unsigned char b = (unsigned char)buf[i];

      if(!first)
        printf(" ");
      first = 0;

      printf("%c%c", hex[(b >> 4) & 0xF], hex[b & 0xF]);
    }

    total += r;
  }

  printf("\n");
  close(fd);
  exit(0);
}