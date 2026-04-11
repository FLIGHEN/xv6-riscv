#include "kernel/types.h"
#include "user/user.h"

void 
print_line(int pid, int argi, char c)
{
  printf("%d: ", pid);
  pause(1);
  printf("arg %d, ", argi);
  pause(1);
  printf("char '%c'\n", c);
  pause(1);
}

void
print_line_sync(int mtx, int pid, int argi, char c)
{
  if(mutex_lock(mtx) < 0){
    fprintf(2, "mutex_lock failed\n");
    exit(1);
  }

  print_line(pid, argi, c);

  if(mutex_unlock(mtx) < 0){
    fprintf(2, "mutex_unlock failed\n");
    exit(1);
  }
}

void
run_unsync(int argc, char **argv)
{
  int i, j;
  int pid = getpid();

  for(i = 1; i < argc; i++){
    for(j = 0; argv[i][j] != '\0'; j++){
      print_line(pid, i, argv[i][j]);
    }
  }
}

void
run_sync(int argc, char **argv, int mtx)
{
  int i, j;
  int pid = getpid();

  for(i = 1; i < argc; i++){
    for(j = 0; argv[i][j] != '\0'; j++){
      print_line_sync(mtx, pid, i, argv[i][j]);
    }
  }
}

int
main(int argc, char **argv)
{
  int pid;
  int cst = 0;
  int mtx;

  if(argc < 2){
    fprintf(2, "usage: mutextest arg1 [arg2 ...]\n");
    exit(1);
  }

  printf("~~~ WITHOUT SYNC ~~~\n");

  pid = fork();
  if(pid < 0){
    fprintf(2, "fork failed\n");
    exit(1);
  }

  if(pid == 0){
    run_unsync(argc, argv);
    exit(0);
  } else {
    run_unsync(argc, argv);
    wait(&cst);
  }

  printf("\n~~~ WITH SYNC ~~~\n");

  mtx = mutex();
  if(mtx < 0){
    fprintf(2, "mutex failed\n");
    exit(1);
  }

  pid = fork();
  if(pid < 0){
    fprintf(2, "fork failed\n");
    close(mtx);
    exit(1);
  }

  if(pid == 0){
    run_sync(argc, argv, mtx);
    close(mtx);
    exit(0);
  } else {
    run_sync(argc, argv, mtx);
    wait(&cst);
    close(mtx);
  }

  exit(0);
}