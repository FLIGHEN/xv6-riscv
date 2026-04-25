#include "kernel/types.h"
#include "user/user.h"
#include "user/dateutil.h"

int
main(void)
{
  int64 t = getrtc();
  print_date(t);
  exit(0);
}

