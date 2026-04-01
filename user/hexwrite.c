#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int
hex_value(char c)
{
  if(c >= '0' && c <= '9')
    return c - '0';
  if(c >= 'a' && c <= 'f')
    return c - 'a' + 10;
  if(c >= 'A' && c <= 'F')
    return c - 'A' + 10;
  return -1;
}

int
main(int argc, char *argv[])
{
  int fd;
  int len;
  int nbytes;
  char *hexs;
  char buf[256];

  if(argc != 3){
    fprintf(2, "usage: hexwrite <hexbytes> <file>\n");
    exit(1);
  }

  hexs = argv[1];
  len = strlen(hexs);

  if(len == 0 || (len % 2) != 0){
    fprintf(2, "hexwrite: invalid hex string\n");
    exit(1);
  }

  nbytes = len / 2;
  if(nbytes > sizeof(buf)){
    fprintf(2, "hexwrite: input too long\n");
    exit(1);
  }

  for(int i = 0; i < nbytes; i++){
    int hi = hex_value(hexs[2 * i]);
    int lo = hex_value(hexs[2 * i + 1]);

    if(hi < 0 || lo < 0){
      fprintf(2, "hexwrite: invalid hex string\n");
      exit(1);
    }

    buf[i] = (char)((hi << 4) | lo);
  }

  fd = open(argv[2], O_WRONLY);
  if(fd < 0){
    fprintf(2, "hexwrite: cannot open %s\n", argv[2]);
    exit(1);
  }

  int w = write(fd, buf, nbytes);
  if(w != nbytes){
    printf("Write error\n");
    close(fd);
    exit(1);
  }

  close(fd);
  exit(0);
}