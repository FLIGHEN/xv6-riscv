// init: The initial user-level program

#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/spinlock.h"
#include "kernel/sleeplock.h"
#include "kernel/fs.h"
#include "kernel/file.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#include "kernel/psdev.h"

char *argv[] = { "sh", 0 };

void
val_mknod_dev(char* name, int major, int minor){
  struct stat st;

  if(stat(name, &st) >= 0)
    return;

  if(mknod(name, major, minor) < 0)
    fprintf(2, "init: mknod %s failed\n", name);
}

int
main(void)
{
  int pid, wpid;

  if(open("console", O_RDWR) < 0){
    mknod("console", CONSOLE, 0);
    open("console", O_RDWR);
  }
  dup(0);  // stdout
  dup(0);  // stderr

  val_mknod_dev("null", PSDEVMAJOR, PSDEV_NULL);
  val_mknod_dev("zero", PSDEVMAJOR, PSDEV_ZERO);
  val_mknod_dev("urandom", PSDEVMAJOR, PSDEV_URANDOM);
  val_mknod_dev("nullstat", PSDEVMAJOR, PSDEV_NULLSTAT);

  for(;;){
    printf("init: starting sh\n");
    pid = fork();
    if(pid < 0){
      printf("init: fork failed\n");
      exit(1);
    }
    if(pid == 0){
      exec("sh", argv);
      printf("init: exec sh failed\n");
      exit(1);
    }

    for(;;){
      // this call to wait() returns if the shell exits,
      // or if a parentless process exits.
      wpid = wait((int *) 0);
      if(wpid == pid){
        // the shell exited; restart it.
        break;
      } else if(wpid < 0){
        printf("init: wait returned an error\n");
        exit(1);
      } else {
        // it was a parentless process; do nothing.
      }
    }
  }
}
