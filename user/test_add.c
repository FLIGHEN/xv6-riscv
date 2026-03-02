#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char **argv)
{
  if(argc != 3){
    fprintf(2, "error: usage - test_add a b\n");
    exit(1);
  }

  int a = atoi(argv[1]);
  int b = atoi(argv[2]);

  int res = add(a, b);
  if(res < 0){
    fprintf(2, "error: sys_add failed\n");
    exit(1);
  }

  printf("%d\n", res);
  exit(0);
}
