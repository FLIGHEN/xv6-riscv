#include "user/dateutil.h"
#include "user/user.h"

void
print2(int x)
{
  if(x < 10)
    printf("0%d", x);
  else
    printf("%d", x);
}

void
print4(int x)
{
  if(x < 10)
    printf("000%d", x);
  else if(x < 100)
    printf("00%d", x);
  else if(x < 1000)
    printf("0%d", x);
  else
    printf("%d", x);
}

void
print9(int x)
{
  if(x < 10)
    printf("00000000%d", x);
  else if(x < 100)
    printf("0000000%d", x);
  else if(x < 1000)
    printf("000000%d", x);
  else if(x < 10000)
    printf("00000%d", x);
  else if(x < 100000)
    printf("0000%d", x);
  else if(x < 1000000)
    printf("000%d", x);
  else if(x < 10000000)
    printf("00%d", x);
  else if(x < 100000000)
    printf("0%d", x);
  else
    printf("%d", x);
}

void
print_date(int64 t){
  int64 sec = t / 1000000000LL;
  int64 nsec = t % 1000000000LL;

  if (nsec < 0) {
    nsec += 1000000000LL;
    sec -= 1;
  }

  int64 days = sec / 86400;
  int64 rem = sec % 86400;

  if (rem < 0) {
    rem += 86400;
    days -= 1;
  }

  int z = days + 719468;
  int era = (z >= 0 ? z : z - 146096) / 146097;
  int doe = z - era * 146097;
  int yoe = (doe - doe/1460 + doe/36524 - doe/146096) / 365;
  int y = yoe + era * 400;
  int doy = doe - (365*yoe + yoe/4 - yoe/100);
  int mp = (5*doy + 2)/153;

  int d = doy - (153*mp+2)/5 + 1;
  int m = mp + (mp < 10 ? 3 : -9);
  y += (m <= 2);

  int hour = rem / 3600;
  rem %= 3600;
  int min = rem / 60;
  int sec2 = rem % 60;

  print2(d);
  printf("-");
  print2(m);
  printf("-");
  print4(y);
  printf(" ");
  print2(hour);
  printf(":");
  print2(min);
  printf(":");
  print2(sec2);
  printf(".");
  print9((int)nsec);
  printf("\n");
}
