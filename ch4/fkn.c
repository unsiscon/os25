#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid1, pid2;

    printf("父进程 PID=%d 开始运行。\n", getpid());

    pid1 = fork();
    if (pid1 == 0) {
        nice(5); // 降低优先级
        for (int i = 0; i < 20; i++) {
            printf("b");
            fflush(stdout);
            usleep(100000);
        }
        exit(0);
    }

    pid2 = fork();
    if (pid2 == 0) {
        nice(-5); // 提高优先级（需要root权限，普通用户可能失败）
        for (int i = 0; i < 20; i++) {
            printf("c");
            fflush(stdout);
            usleep(100000);
        }
        exit(0);
    }

    for (int i = 0; i < 20; i++) {
        printf("a");
        fflush(stdout);
        usleep(100000);
    }

    wait(NULL);
    wait(NULL);
    printf("\n父进程结束。\n");

    return 0;
}
