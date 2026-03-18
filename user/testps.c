#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/procinfo.h"
#include "user/user.h"

void
test_count_proc(void)
{
  int n = ps_listinfo(0, 3);
  if(n > 0)
    printf("test_count_proc: OK (return=%d)\n", n);
  else
    printf("test_count_proc: FAIL (return=%d)\n", n);
}

void
test_proc_more_than_lim(void)
{
  int n = ps_listinfo(0, 0);
  if(n <= 0){
    printf("test_proc_more_than_lim: FAIL (count=%d)\n", n);
    return;
  }

  struct procinfo buf[1];
  int len = ps_listinfo(buf, 1);

  if(n > 1){
    if(len > 1)
      printf("test_proc_more_than_lim: OK (returned %d > 1)\n", len);
    else
      printf("test_proc_more_than_lim: FAIL (returned %d)\n", len);
  } else {
    printf("test_proc_more_than_lim: SKIP (only 1 process)\n");
  }
}

void
test_good_buffer(void)
{
  int n = ps_listinfo(0, 0);
  if(n <= 0){
    printf("test_good_buffer: FAIL (count=%d)\n", n);
    return;
  }

  int lim = n + 4;
  struct procinfo *buf = malloc(lim * sizeof(struct procinfo));
  if(buf == 0){
    printf("test_good_buffer: FAIL (buf malloc)\n");
    return;
  }

  int res = ps_listinfo(buf, lim);
  if(res < 0){
    printf("test_good_buffer: FAIL (return=%d)\n", res);
    free(buf);
    return;
  }

  if(res > lim){
    printf("test_good_buffer: FAIL (return=%d > lim=%d)\n", res, lim);
    free(buf);
    return;
  }

  printf("test_good_buffer: OK (return=%d)\n", res);

  free(buf);
}

void
test_bad_address(void)
{
  int res = ps_listinfo((struct procinfo *)404, 4);
  if(res < 0)
    printf("test_bad_address: OK (return=%d)\n", res);
  else
    printf("test_bad_address: FAIL (return=%d)\n", res);
}

void
test_negative_lim(void)
{
  struct procinfo buf[4];
  int res = ps_listinfo(buf, -1);
  if(res < 0)
    printf("test_negative_lim: OK (return=%d)\n", res);
  else
    printf("test_negative_lim: FAIL (return=%d)\n", res);
}

void
test_resize_loop(void)
{
  int lim = 1;

  while(1){
    struct procinfo *buf = malloc(lim * sizeof(struct procinfo));
    if(buf == 0){
      printf("test_resize_loop: FAIL (buf malloc)\n");
      return;
    }

    int res = ps_listinfo(buf, lim);
    if(res < 0){
      printf("test_resize_loop: FAIL (return=%d)\n", res);
      free(buf);
      return;
    }

    if(res <= lim){
      printf("test_resize_loop: OK (lim=%d, got=%d)\n", lim, res);
      free(buf);
      return;
    }
    printf("test_resize_loop: WAITITNG... (lim=%d, res=%d)\n", lim, res);

    free(buf);
    lim = res;
  }
}

int
main(void)
{
  test_count_proc();
  test_proc_more_than_lim();
  test_good_buffer();
  test_bad_address();
  test_negative_lim();
  test_resize_loop();
  exit(0);
}
