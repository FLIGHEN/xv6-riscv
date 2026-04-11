#include "types.h"
#include "param.h"
#include "riscv.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "defs.h"
#include "file.h"
#include "psdev.h"

struct spinlock psdev_seed_lock;
struct spinlock nullstat_lock;

uint64 psdev_seed = 123456;
uint64 nullstat_total = 0;
static char zero_buf[64] = {0};

uint64
psdev_rand_next(void)
{
  psdev_seed = psdev_seed * 2862933555777941757ULL + 3037000493ULL;
  return psdev_seed;
}

void
psdevinit(void)
{
  initlock(&psdev_seed_lock, "psdev_seed");
  initlock(&nullstat_lock, "nullstat");
  devsw[PSDEVMAJOR].read = psdevread;
  devsw[PSDEVMAJOR].write = psdevwrite;
}

int
psdevread(int minor, int user_dst, uint64 dst, int n)
{
  if(n < 0)
    return -1;

  switch(minor){
  case PSDEV_NULL:
    return 0;

  case PSDEV_ZERO: {
    int total = 0;

    while(total < n){
      int chunk = n - total;
      if(chunk > sizeof(zero_buf))
        chunk = sizeof(zero_buf);

      if(either_copyout(user_dst, dst + total, zero_buf, chunk) < 0)
        return -1;

      total += chunk;
    }

    return total;
  }

  case PSDEV_URANDOM: {
    char buf[64];
    int total = 0;

    while(total < n){
      int chunk = n - total;
      if(chunk > sizeof(buf))
        chunk = sizeof(buf);

      acquire(&psdev_seed_lock);
      for(int i = 0; i < chunk; i++){
        buf[i] = (char)(psdev_rand_next() & 0xFF);
      }
      release(&psdev_seed_lock);

      if(either_copyout(user_dst, dst + total, buf, chunk) < 0)
        return -1;

      total += chunk;
    }

    return total;
  }

  case PSDEV_NULLSTAT: {
    uint64 value;

    if(n != sizeof(uint64))
      return -1;

    acquire(&nullstat_lock);
    value = nullstat_total;
    release(&nullstat_lock);

    if(either_copyout(user_dst, dst, (char *)&value, sizeof(uint64)) < 0)
      return -1;

    return sizeof(uint64);
  }

  default:
    return -1;
  }
}

int
psdevwrite(int minor, int user_src, uint64 src, int n)
{
  if(n < 0)
    return -1;

  switch(minor){
  case PSDEV_NULL:
    return n;

  case PSDEV_ZERO:
    return -1;

  case PSDEV_URANDOM: {
    uint64 new_seed;

    if(n != sizeof(uint64))
      return -1;

    if(either_copyin((char *)&new_seed, user_src, src, sizeof(uint64)) < 0)
      return -1;

    acquire(&psdev_seed_lock);
    psdev_seed = new_seed;
    release(&psdev_seed_lock);

    return sizeof(uint64);
  }

  case PSDEV_NULLSTAT:
    acquire(&nullstat_lock);
    nullstat_total += n;
    release(&nullstat_lock);

    return n;

  default:
    return -1;
  }
}