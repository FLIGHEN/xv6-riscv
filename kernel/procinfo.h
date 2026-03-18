#ifndef _PROCINFOH
#define _PROCINFOH

struct procinfo {
  int pid;
  int ppid;
  char name[16];
  char pname[16];
  int state;
};

#endif
