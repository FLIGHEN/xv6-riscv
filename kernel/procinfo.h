#ifndef _PROCINFOH
#define _PROCINFOH

enum procstate { UNUSED, USED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

struct procinfo {
  int pid;
  int ppid;
  char name[16];
  char pname[16];
  int state;
};

#endif
