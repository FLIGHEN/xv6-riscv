#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/dmesg.h"
#include "user/user.h"

void
usage(void)
{
  fprintf(2, "usage: logctl {syscall|intr|proc|exec|all} {on|off} [ticks]\n");
}

int
parse_class(char *s)
{
  if(strcmp(s, "syscall") == 0)
    return LOG_SYSCALL;
  if(strcmp(s, "intr") == 0)
    return LOG_INTR;
  if(strcmp(s, "proc") == 0)
    return LOG_PROC;
  if(strcmp(s, "exec") == 0)
    return LOG_EXEC;
  if(strcmp(s, "all") == 0)
    return LOG_ALL;
  return 0;
}

int
main(int argc, char *argv[])
{
  int mask;
  int enable;
  int duration;

  if(argc != 3 && argc != 4){
    usage();
    exit(1);
  }

  mask = parse_class(argv[1]);
  if(mask == 0){
    usage();
    exit(1);
  }

  if(strcmp(argv[2], "on") == 0){
    enable = 1;
  } else if(strcmp(argv[2], "off") == 0){
    enable = 0;
  } else {
    usage();
    exit(1);
  }

  duration = 0;
  if(argc == 4){
    if(argv[3][0] == '-'){
      usage();
      exit(1);
    }
    duration = atoi(argv[3]);
  }

  if(logctl(mask, enable, duration) < 0){
    fprintf(2, "logctl: syscall failed\n");
    exit(1);
  }

  exit(0);
}
