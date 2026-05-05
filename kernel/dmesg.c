#include <stdarg.h>

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"
#include "dmesg.h"

#define DMESG_SIZE (DMESG_PAGES * PGSIZE)

char digits[] = "0123456789abcdef";

struct {
  struct spinlock lock;
  char data[DMESG_SIZE];
  uint head;
  uint tail;
  int wrapped;
} dmesg;

struct {
  struct spinlock lock;
  int mask;
  uint deadline[4];
} logstate;

uint
dmesg_ticks(void)
{
  uint t;

  acquire(&tickslock);
  t = ticks;
  release(&tickslock);

  return t;
}

int
log_index(int class)
{
  if(class == LOG_SYSCALL)
    return 0;
  if(class == LOG_INTR)
    return 1;
  if(class == LOG_PROC)
    return 2;
  if(class == LOG_EXEC)
    return 3;
  return -1;
}

void
dmesg_putc_l(char c)
{
  dmesg.data[dmesg.head] = c;
  dmesg.head = (dmesg.head + 1) % DMESG_SIZE;

  if(dmesg.head == dmesg.tail){
    dmesg.tail = (dmesg.tail + 1) % DMESG_SIZE;
    dmesg.wrapped = 1;
  }
}

void
dmesg_printint_l(long long inp, int base, int as_sign)
{
  char buf[20];
  int i;
  unsigned long long num;

  int sign = (inp < 0);

  if(as_sign && sign){
    num = -inp;
  }
  else{
    num = inp;
  }

  i = 0;
  do {
    buf[i++] = digits[num % base];
  } while((num /= base) != 0);

  if(sign)
    buf[i++] = '-';

  while(--i >= 0)
    dmesg_putc_l(buf[i]);
}

void
dmesg_printptr_l(uint64 x)
{
  int i;

  dmesg_putc_l('0');
  dmesg_putc_l('x');
  for(i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4)
    dmesg_putc_l(digits[x >> (sizeof(uint64) * 8 - 4)]);
}

void
dmesg_vprintf_l(const char *fmt, va_list ap)
{
  int i, cx, c0, c1, c2;
  char *s;

  for(i = 0; (cx = fmt[i] & 0xff) != 0; i++){
    if(cx != '%'){
      dmesg_putc_l(cx);
      continue;
    }

    i++;
    c0 = fmt[i] & 0xff;
    c1 = c2 = 0;
    if(c0)
      c1 = fmt[i+1] & 0xff;
    if(c1)
      c2 = fmt[i+2] & 0xff;

    if(c0 == 'd'){
      dmesg_printint_l(va_arg(ap, int), 10, 1);
    } else if(c0 == 'l' && c1 == 'd'){
      dmesg_printint_l(va_arg(ap, uint64), 10, 1);
      i += 1;
    } else if(c0 == 'l' && c1 == 'l' && c2 == 'd'){
      dmesg_printint_l(va_arg(ap, uint64), 10, 1);
      i += 2;
    } else if(c0 == 'u'){
      dmesg_printint_l(va_arg(ap, uint32), 10, 0);
    } else if(c0 == 'l' && c1 == 'u'){
      dmesg_printint_l(va_arg(ap, uint64), 10, 0);
      i += 1;
    } else if(c0 == 'l' && c1 == 'l' && c2 == 'u'){
      dmesg_printint_l(va_arg(ap, uint64), 10, 0);
      i += 2;
    } else if(c0 == 'x'){
      dmesg_printint_l(va_arg(ap, uint32), 16, 0);
    } else if(c0 == 'l' && c1 == 'x'){
      dmesg_printint_l(va_arg(ap, uint64), 16, 0);
      i += 1;
    } else if(c0 == 'l' && c1 == 'l' && c2 == 'x'){
      dmesg_printint_l(va_arg(ap, uint64), 16, 0);
      i += 2;
    } else if(c0 == 'p'){
      dmesg_printptr_l(va_arg(ap, uint64));
    } else if(c0 == 'c'){
      dmesg_putc_l(va_arg(ap, uint));
    } else if(c0 == 's'){
      if((s = va_arg(ap, char*)) == 0)
        s = "(null)";
      for(; *s; s++)
        dmesg_putc_l(*s);
    } else if(c0 == '%'){
      dmesg_putc_l('%');
    } else if(c0 == 0){
      break;
    } else {
      dmesg_putc_l('%');
      dmesg_putc_l(c0);
    }
  }
}

void
dmesginit(void)
{
  initlock(&dmesg.lock, "dmesg");
  dmesg.head = 0;
  dmesg.tail = 0;
  dmesg.wrapped = 0;

  initlock(&logstate.lock, "logctl");
  logstate.mask = 0;
  for(int i = 0; i < NELEM(logstate.deadline); i++)
    logstate.deadline[i] = 0;
}

void
pr_msg(const char *fmt, ...)
{
  va_list ap;
  uint t;

  t = dmesg_ticks();

  acquire(&dmesg.lock);
  dmesg_putc_l('[');
  dmesg_printint_l(t, 10, 0);
  dmesg_putc_l(']');
  dmesg_putc_l(' ');

  va_start(ap, fmt);
  dmesg_vprintf_l(fmt, ap);
  va_end(ap);

  dmesg_putc_l('\n');
  release(&dmesg.lock);
}

int
dmesg_copyout(uint64 dst, int size)
{
  struct proc *p = myproc();
  uint pos;
  int copied;

  if(size <= 0)
    return -1;

  acquire(&dmesg.lock);

  pos = dmesg.tail;
  if(dmesg.wrapped && dmesg.head != dmesg.tail){
    while(pos != dmesg.head && dmesg.data[pos] != '\n')
      pos = (pos + 1) % DMESG_SIZE;
    if(pos != dmesg.head)
      pos = (pos + 1) % DMESG_SIZE;
  }

  copied = 0;
  while(pos != dmesg.head && copied < size - 1){
    if(copyout(p->pagetable, dst + copied, &dmesg.data[pos], 1) < 0){
      release(&dmesg.lock);
      return -1;
    }
    copied++;
    pos = (pos + 1) % DMESG_SIZE;
  }

  char nul = '\0';
  if(copyout(p->pagetable, dst + copied, &nul, 1) < 0){
    release(&dmesg.lock);
    return -1;
  }

  release(&dmesg.lock);
  return copied;
}

void
logctl_set(int mask, int enable, int duration)
{
  uint now;

  mask &= LOG_ALL;
  now = 0;
  if(enable && duration > 0)
    now = dmesg_ticks();

  acquire(&logstate.lock);
  for(int bit = LOG_SYSCALL; bit <= LOG_EXEC; bit <<= 1){
    int idx;

    if((mask & bit) == 0)
      continue;

    idx = log_index(bit);
    if(enable){
      logstate.mask |= bit;
      logstate.deadline[idx] = duration > 0 ? now + duration : 0;
    } else {
      logstate.mask &= ~bit;
      logstate.deadline[idx] = 0;
    }
  }
  release(&logstate.lock);
}

int
log_enabled(int class)
{
  uint now;
  int have_ticks;
  int ret;

  class &= LOG_ALL;
  if(class == 0)
    return 0;

  now = 0;
  have_ticks = 0;

  acquire(&logstate.lock);
  for(int bit = LOG_SYSCALL; bit <= LOG_EXEC; bit <<= 1){
    int idx;

    if((class & bit) == 0 || (logstate.mask & bit) == 0)
      continue;

    idx = log_index(bit);
    if(logstate.deadline[idx] != 0){
      if(!have_ticks){
        now = dmesg_ticks();
        have_ticks = 1;
      }
      if(now >= logstate.deadline[idx]){
        logstate.mask &= ~bit;
        logstate.deadline[idx] = 0;
      }
    }
  }
  ret = (logstate.mask & class) != 0;
  release(&logstate.lock);

  return ret;
}
