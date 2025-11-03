#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid1, pid2;

    printf("父进程开始运行，PID=%d\n", getpid());

    // 创建第一个子进程
    pid1 = fork();
    if (pid1 < 0) {
        perror("fork1 failed");
        exit(1);
    }
    if (pid1 == 0) {
        // 第一个子进程执行区域
        for (int i = 0; i < 10; i++) {
            printf("b"); // 子进程1输出 b
            fflush(stdout);
            usleep(100000); // 延时 0.1 秒，方便观察交替执行
        }
        exit(0);
    }

    // 创建第二个子进程
    pid2 = fork();
    if (pid2 < 0) {
        perror("fork2 failed");
        exit(1);
    }
    if (pid2 == 0) {
        // 第二个子进程执行区域
        for (int i = 0; i < 10; i++) {
            printf("c"); // 子进程2输出 c
            fflush(stdout);
            usleep(100000);
        }
        exit(0);
    }

    // 父进程区域
    for (int i = 0; i < 10; i++) {
        printf("a"); // 父进程输出 a
        fflush(stdout);
        usleep(100000);
    }

    // 等待子进程结束
    wait(NULL);
    wait(NULL);
    printf("\n父进程结束。\n");

    return 0;
}
