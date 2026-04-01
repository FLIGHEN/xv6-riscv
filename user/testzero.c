#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int
main(void)
{
  int fd = open("zero", O_WRONLY);
  if(fd < 0){
    printf("open failed\n");
    exit(1);
  }

  int r = write(fd, "abc", 3);

  if(r < 0){
    printf("write error (success!)\n");
  } else {
    printf("write ok (fail!): %d\n", r);
  }

  close(fd);
  exit(0);
}