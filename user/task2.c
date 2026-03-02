#include "kernel/types.h"
#include "user/user.h"

int write_fd(int fd, const char* arg, int len){
	int offset = 0;

	while (offset < len){
		int bytes_wrtn = write(fd, arg + offset, len - offset);
		if (bytes_wrtn <= 0) 
			return -1;
		offset += bytes_wrtn;
	}

	return 0;
}


int main(int argc, char* argv[]){
	int pipefd[2];
	if(pipe(pipefd) < 0){
    		printf("pipe failed\n");
  	  	exit(1);
  	}

	int pid = fork();
	switch(pid){
		case -1:
			fprintf(2, "fork failed\n");
        		close(pipefd[0]);
        		close(pipefd[1]);
        		exit(1);

        	case 0:
			if(close(pipefd[1]) < 0){
                                fprintf(2, "pipefd[1] close failed\n");
                                exit(1);
                        }

			if(close(0) < 0){
                                fprintf(2, "stdin close failed\n");
 				exit(1);
                        }

			if(dup(pipefd[0]) < 0){
				fprintf(2, "dup failed\n");
				close(pipefd[0]);
				exit(1);
			}

			if(close(pipefd[0]) < 0){
                                fprintf(2, "pipefd[0] close failed\n");
                                exit(1);
                        }

			char *wc_argv[] = {"/wc", 0};
			exec("/wc", wc_argv);

			fprintf(2, "exec failed");
			exit(1);

		default:
			if(close(pipefd[0]) < 0){
                                fprintf(2, "pipefd[0] close failed\n");
                                kill(pid);
                                wait(0);
                                exit(1);
                        }
			
			for(int i = 1; i < argc; i++){
				if(write_fd(pipefd[1], argv[i], strlen(argv[i])) != 0){
					fprintf(2, "pipefd[1] write failed\n");
					close(pipefd[1]);
					kill(pid);
					wait(0);
					exit(1);
				}

				if(write_fd(pipefd[1], "\n", 1) != 0){
                                        fprintf(2, "pipefd[1] write failed\n");
                                        close(pipefd[1]);
                                        kill(pid);
                                        wait(0);
                                        exit(1);
                                }
			}

			if(close(pipefd[1]) < 0){
				fprintf(2, "pipefd[1] close failed\n");
    				kill(pid);
    				wait(0);
    				exit(1);
			}
			
			int status = 0;
			int pid = wait(&status);

			if(pid < 0){
				fprintf(2, "wait failed\n");
				exit(1);
			}

			if(status != 0){
				fprintf(2, "child exit error = %d", status);
				exit(1);
			}

			exit(0);
	}
}
