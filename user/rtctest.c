#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{
  uint64 t = getrtc();
  uint high = (uint)(t >> 32);
  uint low  = (uint)t;

  printf("rtc high=0x%x low=0x%x\n", high, low);
  exit(0);
}