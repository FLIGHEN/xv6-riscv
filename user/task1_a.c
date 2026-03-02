#include "kernel/types.h"
#include "user/user.h"

int main(void){
	int pid;

	pid = fork();

	switch(pid){
		case -1:
			printf("fork error\n");
               		exit(1);
		case 0:
			pause(100 * 15);
			exit(1);
		default:
			printf("parent! parent pid=%d, child=%d\n", getpid(), pid);
			int status = -1;
			
			int finished = wait(&status);
			
	 		printf("child pid finished=%d\n", finished);
			printf("exit code=%d\n", status);

			exit(0);
	}

}




