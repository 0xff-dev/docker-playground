#include <unistd.h>
#include <stdio.h>

//  在fork后, 父进程监控子进程, 避免成为孤儿进程
int main() {
    pid_t fpid;
    fpid = fork();
    if (fpid < 0) {
        printf("Error Fork");
    } else if (fpid == 0 ) {
        printf("Child : %d\n", getpid());
    } else {
        printf("Parent: %d\n", getpid());
    }
    return 0;
}