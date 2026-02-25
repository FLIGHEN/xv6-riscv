#include "kernel/types.h"
#include "user/user.h"
#define MAX 128

#define INT_MAX 2147483647
#define INT_MIN (-2147483647 - 1)

int check_is_number(const char *s){
    if(s == 0 || *s == '\0')
        return 0;

    for(int i = 0; s[i] != '\0'; i++){
        if(s[i] < '0' || s[i] > '9')
            return 0;
    }

    return 1;
}

int main(int argc, char **argv){
    char buf[MAX];
    char *space_pos = 0;

    int i, cc;
    char c = 0;

    int space_count = 0;

    for(i=0; i+1 < MAX;){
        cc = read(0, &c, 1);
        if(cc < 0){
            printf("error: read failed\n");
            exit(1);
        }

        if(cc == 0)
            break;
        if(c == '\n' || c == '\r')
            break;

        buf[i++] = c;

        if(c == ' '){
            space_pos = &buf[i-1];
            
            space_count++;
            if(space_count > 1){
                printf("error: the format is incorrect ( > 1 spaces). There must be one space.\n");
                exit(1);
            }
        }
    }
    if(i + 1 >= MAX && !(c == '\n' || c == '\r')){
        printf("error: input line is too long ( length > %d)\n", MAX - 1);
        exit(1);
    }

    buf[i] = '\0';

    if(i == 0){
        printf("error: input is empty\n");
        exit(1);
    }

    printf("|%s|\n", buf);

    if(space_count == 0){
        printf("error: the format is incorrect ( = 0 spaces). There must be one space.\n");
        exit(1);
    }

    if(space_pos == buf){
        printf("error: no first number\n");
        exit(1);
    }
    if(*(space_pos + 1) == '\0'){
        printf("error: no second number\n");
        exit(1);
    }

    *space_pos = '\0';

    if(!check_is_number(buf)){
        printf("error: first is not a non-negative number\n");
        exit(1);
    }

    if(!check_is_number(space_pos+1)){
        printf("error: second is not a non-negative number\n");
        exit(1);
    }

    int a = atoi(buf);
    int b = atoi(space_pos + 1);

    if( ( b > 0 && a > INT_MAX - b ) || ( b < 0 && a < INT_MIN - b ) ){
        printf("error: overflow\n");
        exit(1);
    }

    printf("%d\n", a + b);

    exit(0);
}

