#include "types.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "riscv.h"
#include "param.h"
#include "proc.h"
#include "defs.h"
#include "fs.h"
#include "file.h"

struct mutex {
  struct sleeplock lock;
};

int
mutexalloc(struct file **f)
{
  struct file *fp;
  struct mutex *m;

  if((fp = filealloc()) == 0)
    return -1;

  m = (struct mutex*)kalloc();

  if(m == 0){
    fileclose(fp);
    return -1;
  }

  initsleeplock(&m->lock, "mutex");

  fp->type = FD_MUTEX;
  fp->readable = 0;
  fp->writable = 0;
  fp->mutex = m;

  *f = fp;

  printf("mutexalloc: m=%p\n", m);

  return 0;
}

void
mutexclose(struct mutex *m)
{
  if(m == 0)
    return;

  printf("mutexclose: m=%p\n", m);

  kfree(m);
}

int
mutexlock(struct file *f)
{
  struct mutex *m;

  if(f == 0 || f->type != FD_MUTEX)
    return -1;

  m = f->mutex;

  acquire(&m->lock.lk);
  if(m->lock.locked && m->lock.pid == myproc()->pid){
    release(&m->lock.lk);
    return -1;
  }
  release(&m->lock.lk);

  acquiresleep(&m->lock);
  return 0;
}

int
mutexunlock(struct file *f)
{
  struct mutex *m;

  if(f == 0 || f->type != FD_MUTEX)
    return -1;

  m = f->mutex;

  acquire(&m->lock.lk);
  if(!m->lock.locked || m->lock.pid != myproc()->pid){
    release(&m->lock.lk);
    return -1;
  }
  release(&m->lock.lk);

  releasesleep(&m->lock);
  return 0;
}

void
mutexunlockifheld(struct file *f)
{
  struct mutex *m;
  int held;

  if(f == 0 || f->type != FD_MUTEX)
    return;

  m = f->mutex;

  acquire(&m->lock.lk);
  held = m->lock.locked && (m->lock.pid == myproc()->pid);
  release(&m->lock.lk);

  if(held)
    releasesleep(&m->lock);
}



