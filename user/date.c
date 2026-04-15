#include "kernel/types.h"
#include "user/user.h"

int
is_vis(int year)
{
  if(year % 400 == 0)
    return 1;
  if(year % 100 == 0)
    return 0;
  return year % 4 == 0;
}

int
days_in_year(int year)
{
  return is_vis(year) ? 366 : 365;
}

int
days_in_month(int year, int month)
{
  static int mdays[12] = {
    31, 28, 31, 30, 31, 30,
    31, 31, 30, 31, 30, 31
  };

  if(month == 2 && is_vis(year))
    return 29;

  return mdays[month - 1];
}

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

int
main(void)
{
  uint64 t = getrtc();
  uint64 secs = t / 1000000000ULL;
  uint64 days = secs / 86400;
  uint64 r = secs % 86400;

  int year = 1970;
  int month = 1;
  int day;
  int hour, minute, second;

  while(days >= (uint64)days_in_year(year)){
    days -= days_in_year(year);
    year++;
  }

  while(days >= (uint64)days_in_month(year, month)){
    days -= days_in_month(year, month);
    month++;
  }

  day = (int)days + 1;

  hour = r / 3600;
  r %= 3600;
  minute = r / 60;
  second = r % 60;

  print2(day);
  printf("-");
  print2(month);
  printf("-");
  print4(year);
  printf(" ");
  print2(hour);
  printf(":");
  print2(minute);
  printf(":");
  print2(second);
  printf("\n");

  exit(0);
}
