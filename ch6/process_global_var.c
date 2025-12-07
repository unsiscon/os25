#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int shared = 100;

int main() {
    pid_t pid; 
    printf("程序开始时, 全局变量 shared 的初始值: %d\n", shared);
    pid = fork();
    if (pid < 0) {
        printf("fork failed\n");
        exit(1); 
    } 
    else if (pid == 0) {
        printf("--- 子进程开始执行 ---\n");
        printf("子进程读取到的初始 shared 值: %d\n", shared);
        // 循环对 shared 变量进行减法操作
        for (int i = 0; i < 5; i++) {
            shared -= 10;
            printf("子进程: shared = %d\n", shared);
            sleep(1);
        }
        printf("--- 子进程执行结束 ---\n");
        exit(0);
    } 
    else {
        printf("--- 父进程开始执行 ---\n");
        printf("父进程读取到的初始 shared 值: %d\n", shared);
        for (int i = 0; i < 5; i++) {
            shared += 10;
            printf("父进程: shared = %d\n", shared);
            sleep(1);
        }
        wait(NULL); 
        printf("父进程结束后, shared 的最终值是: %d\n", shared);
        printf("--- 父进程执行结束 ---\n");
    }
    return 0;
}