#include "kernel/types.h"
#include "kernel/procinfo.h"
#include "user/user.h"

int
main(void)
{
  int lim = ps_listinfo(0, 0);
  if(lim < 0){
    fprintf(2, "error: syscall failed (return=%d)\n", lim);
    exit(1);
  }

  int res_count = 0;

  struct procinfo *buf;

  while(1){
    buf = malloc(lim * sizeof(struct procinfo));
    if(buf == 0){
      printf("error: buf malloc failed\n");
      exit(1);
    }

    res_count = ps_listinfo(buf, lim);
    if(res_count < 0){
      printf("error: syscall failed (return=%d)\n", res_count);
      free(buf);
      exit(1);
    }

    if(res_count <= lim){
      break;
    }

    free(buf);
    lim = res_count;
  }

  printf("all running processes:\n");

  for(int i = 0; i < res_count; i++){
    if(buf[i].ppid != 0)
    {
      printf("  pid=%d ppid=%d state=%d name=%s pname=%s\n",
           buf[i].pid, buf[i].ppid, buf[i].state, buf[i].name, buf[i].pname);
    }
    else{
      printf("  pid=%d ppid=%d state=%d name=%s\n",
            buf[i].pid, buf[i].ppid, buf[i].state, buf[i].name);
    }
  }

  free(buf);
  exit(0);
}
