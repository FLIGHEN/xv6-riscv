#include "kernel/types.h"
#include "user/user.h"
#include "user/dateutil.h"

int
main(void)
{
  printf("Expected: 01-01-1970 00:00:00.000000000\n");
  print_date(0);
  printf("\n");

  printf("Expected: 31-12-1969 23:59:59.999999999\n");
  print_date(-1);
  printf("\n");

  printf("Expected: 31-12-1969 23:59:59.000000000\n");
  print_date(-1000000000LL);
  printf("\n");

  printf("Expected: 31-12-1969 00:00:00.000000000\n");
  print_date(-86400000000000LL);
  printf("\n");

  exit(0);
}