#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "memlayout.h"

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

uint64
rtc_read_time(void)
{
  uint low, high;

  low = rtc_read_low();
  high = rtc_read_high();

  return ((uint64)high << 32) | low;
}
