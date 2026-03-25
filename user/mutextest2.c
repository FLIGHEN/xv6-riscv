#include "kernel/types.h"
#include "user/user.h"

void
test_read_write(void)
{
  int m;
  char buf[16];
  int r, w;

  printf("\n~~~ test_read_write ~~~\n");

  m = mutex();
  if(m < 0){
    printf("[FAIL] mutex() failed\n");
    return;
  }

  r = read(m, buf, sizeof(buf));
  w = write(m, buf, sizeof(buf));

  printf("read returned: %d\n", r);
  printf("write returned: %d\n", w);

  close(m);

  if(r < 0)
    printf("read: [OK]\n");
  else
    printf("[FAIL] read on mutex did not fail\n");

  if(w < 0)
    printf("write: [OK]\n");
  else
    printf("[FAIL] write on mutex did not fail\n");
}

void
test_unlock_by_other(void)
{
  int m, pid, s = 0;

  printf("\n~~~ test_unlock_by_other ~~~\n");

  m = mutex();
  if(m < 0){
    printf("[FAIL] mutex() failed\n");
    return;
  }

  if(mutex_lock(m) < 0){
    printf("[FAIL] parent mutex_lock failed\n");
    close(m);
    return;
  }

  pid = fork();
  if(pid < 0){
    printf("[FAIL] fork failed\n");
    mutex_unlock(m);
    close(m);
    return;
  }

  if(pid == 0){
    int r = mutex_unlock(m);
    if(r < 0){
        printf("child: mutex_unlock returned %d\n", r);
        exit(0);
    }
    printf("child: mutex_unlock returned %d\n", r);

    exit(1);
  }

  wait(&s);

  mutex_unlock(m);
  close(m);

  if(s == 0)
    printf("[OK]\n");
  else
    printf("[FAIL] child unlocked parent's mutex\n");
}

void
test_close_by_owner(void)
{
  int m, pid, s = 0;

  printf("\n~~~ test_close_by_owner ~~~\n");

  m = mutex();
  if(m < 0){
    printf("[FAIL] mutex() failed\n");
    return;
  }

  if(mutex_lock(m) < 0){
    printf("[FAIL] parent mutex_lock failed\n");
    close(m);
    return;
  }

  pid = fork();
  if(pid < 0){
    printf("[FAIL] fork failed\n");
    mutex_unlock(m);
    close(m);
    return;
  }

  if(pid == 0){
    pause(10);
    if(mutex_lock(m) < 0){
      printf("child: mutex_lock failed\n");
      exit(1);
    }
    printf("child: managed to lock mutex, unlocking...\n");
    mutex_unlock(m);
    close(m);
    exit(0);
  }

  printf("parent: calling close(m) while owning mutex\n");
  close(m);

  wait(&s);

  if(s == 0)
    printf("[OK]\n");
  else
    printf("[FAIL] close by owner did not release mutex\n");
}

void
test_close_by_other(void)
{
  int m, pid, s = 0;

  printf("\n~~~ test_close_by_other ~~~\n");

  m = mutex();
  if(m < 0){
    printf("[FAIL] mutex() failed\n");
    return;
  }

  pid = fork();
  if(pid < 0){
    printf("[FAIL] fork failed\n");
    close(m);
    return;
  }

  if(pid == 0){
    if(mutex_lock(m) < 0){
      printf("child: mutex_lock failed\n");
      exit(1);
    }
    printf("child: locked mutex\n");
    pause(10);
    if(mutex_unlock(m) < 0){
      printf("child: mutex_unlock failed\n");
      exit(1);
    }
    printf("child: unlocked mutex\n");
    close(m);
    exit(0);
  }

  pause(2);
  printf("parent: close(m) while child owns mutex\n");
  close(m);

  wait(&s);

  if(s == 0)
    printf("[OK]\n");
  else
    printf("[FAIL] close by non-owner broke mutex\n");
}

void
test_exit_with_open_mutex(void)
{
  int m, pid, s = 0;

  printf("\n~~~ test_exit_with_open_mutex ~~~\n");

  m = mutex();
  if(m < 0){
    printf("[FAIL] mutex() failed\n");
    return;
  }

  pid = fork();
  if(pid < 0){
    printf("[FAIL] fork failed\n");
    close(m);
    return;
  }

  if(pid == 0){
    if(mutex_lock(m) < 0){
      printf("child: mutex_lock failed\n");
      exit(1);
    }
    printf("child: exiting without unlocking/exiting mutex\n");
    exit(0);
  }

  wait(&s);
  if(s != 0){
    printf("[FAIL] child exited with error\n");
    close(m);
    return;
  }

  if(mutex_lock(m) < 0){
    printf("[FAIL] parent could not lock mutex after child exit\n");
    close(m);
    return;
  }

  printf("parent: locked mutex after child exit\n");
  mutex_unlock(m);
  close(m);
  printf("[OK]\n");
}

int
main(void)
{
  printf("~~~ test start ~~~\n");

  test_read_write();
  test_unlock_by_other();
  test_close_by_owner();
  test_close_by_other();
  test_exit_with_open_mutex();

  printf("\n~~~ done! ~~~\n");
  exit(0);
}