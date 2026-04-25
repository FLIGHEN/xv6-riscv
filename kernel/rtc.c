#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "memlayout.h"
#include "spinlock.h"

struct spinlock rtclock;

void
rtcinit(void)
{
  initlock(&rtclock, "rtc");
}

uint
rtc_reg_read(uint64 addr)
{
  return *(volatile uint*)addr;
}

uint
rtc_read_low(void)
{
  return rtc_reg_read(RTC_LOW);
}

uint
rtc_read_high(void)
{
  return rtc_reg_read(RTC_HIGH);
}

int64
rtc_read_time(void)
{
  uint low, high;
  uint64 raw;

  acquire(&rtclock);
  low = rtc_read_low();
  high = rtc_read_high();
  release(&rtclock);

  raw = ((uint64)high << 32) | low;
  return (int64)raw;
}
