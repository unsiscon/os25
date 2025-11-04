#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main() {
    pid_t pid1, pid2, pid3;

    printf("根进程开始运行，PID=%d，父进程PID=%d\n", getpid(), getppid());

    // 第一级：创建3个子进程
    pid1 = fork();
    if (pid1 == 0) {
        // 子进程 P1
        printf("子进程 P1 运行中，PID=%d，父进程PID=%d\n", getpid(), getppid());

        pid_t pid4 = fork();
        if (pid4 == 0) {
            printf("子进程 P4 运行中，PID=%d，父进程PID=%d\n", getpid(), getppid());
            exit(0);
        }

        pid_t pid5 = fork();
        if (pid5 == 0) {
            printf("子进程 P5 运行中，PID=%d，父进程PID=%d\n", getpid(), getppid());
            exit(0);
        }

        // 等待两个子进程结束
        wait(NULL);
        wait(NULL);
        exit(0);
    }

    pid2 = fork();
    if (pid2 == 0) {
        // 子进程 P2
        printf("子进程 P2 运行中，PID=%d，父进程PID=%d\n", getpid(), getppid());
        exit(0);
    }

    pid3 = fork();
    if (pid3 == 0) {
        // 子进程 P3
        printf("子进程 P3 运行中，PID=%d，父进程PID=%d\n", getpid(), getppid());

        pid_t pid6 = fork();
        if (pid6 == 0) {
            printf("子进程 P6 运行中，PID=%d，父进程PID=%d\n", getpid(), getppid());
            exit(0);
        }

        wait(NULL);
        exit(0);
    }

    // 根进程等待所有子进程结束
    wait(NULL);
    wait(NULL);
    wait(NULL);

    printf("根进程结束，PID=%d\n", getpid());
    return 0;
}
