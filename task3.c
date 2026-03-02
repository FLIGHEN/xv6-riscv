#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define BUFF_SIZE 4096

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
    pid_t pid;

    int pipefd[2];
    if(pipe(pipefd) < 0){
        fprintf(stderr, "pipe failed\n");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    switch (pid) {
    case -1:
        fprintf(stderr, "fork failed\n");
        exit(EXIT_FAILURE);
    case 0:
        if(close(pipefd[1]) < 0){
            fprintf(stderr, "pipefd[1] close failed\n");
            exit(EXIT_FAILURE);
        }

        char buffer[BUFF_SIZE];

        ssize_t bytes_read = read(pipefd[0], buffer, BUFF_SIZE);

        while(bytes_read > 0){
            if(write_fd(1, buffer, bytes_read) != 0){
                fprintf(stderr, "write stdout failed\n");
                exit(1);
            }

            bytes_read = read(pipefd[0], buffer, BUFF_SIZE);
        }

        if(bytes_read == -1){
            fprintf(stderr, "pipefd[0] read failed\n");
            exit(EXIT_FAILURE);
        }

        if (close(pipefd[0]) < 0) {
            fprintf(stderr, "pipefd[0] close failed\n");
            exit(1);
        }

        exit(EXIT_SUCCESS);
    default:
        if (close(pipefd[0]) < 0) {
            fprintf(stderr, "pipefd[0] close failed\n");
            kill(pid, SIGKILL);
            wait(0);
            exit(1);
        }

        for (int i = 1; i < argc; i++) {
            if (write_fd(pipefd[1], argv[i], strlen(argv[i])) < 0) {
                fprintf(stderr, "pipefd[1] write failed\n");
                close(pipefd[1]);
                kill(pid, SIGKILL);
                wait(0);
                exit(1);
            }

            if (write_fd(pipefd[1], "\n", 1) < 0) {
                fprintf(stderr, "pipefd[1] write failed\n");
                close(pipefd[1]);
                kill(pid, SIGKILL);
                wait(0);
                exit(EXIT_FAILURE);
            }
        }

        if (close(pipefd[1]) < 0) {
            fprintf(stderr, "pipefd[1] close failed\n");
            kill(pid, SIGKILL);
            wait(0);
            exit(EXIT_FAILURE);
        }

        int status = 0;
        int child_pid = wait(&status);

        if (child_pid < 0) {
            fprintf(stderr, "wait failed\n");
            exit(EXIT_FAILURE);
        }

        if (status != 0) {
            fprintf(stderr, "child exit error = %d\n", status);
            exit(EXIT_FAILURE);
        }

        exit(EXIT_SUCCESS);
    }
}
