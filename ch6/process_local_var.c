#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h> // 引入 wait() 函数所需的头文件

/*
 * 功能: 演示父子进程操作函数内局部变量
 * 描述: 
 *   - 在 main 函数内定义一个局部变量 shared。
 *   - 使用 fork() 创建子进程。
 *   - 父进程对 shared 进行循环加法操作。
 *   - 子进程对 shared 进行循环减法操作。
 *   - 观察并分析父子进程的输出结果。
 */
int main() {
    // 1. 在 main() 函数中定义一个局部变量
    int shared = 100; 

    pid_t pid; // 用于存储 fork() 的返回值

    // 2. 使用 fork() 创建子进程
    pid = fork();

    // fork() 错误处理
    if (pid < 0) {
        printf("fork failed\n");
        exit(1); // 创建失败，退出程序
    } 
    // 子进程代码块 (fork() 返回值为 0)
    else if (pid == 0) {
        printf("--- 子进程开始执行 ---\n");
        printf("子进程读取到的初始 shared 值: %d\n", shared);
        
        // 循环对 shared 变量进行减法操作
        for (int i = 0; i < 5; i++) {
            shared -= 10;
            printf("子进程: shared = %d\n", shared);
            sleep(1); // 暂停1秒，以便观察交替执行
        }
        printf("--- 子进程执行结束 ---\n");
        exit(0); // 子进程执行完毕，正常退出
    } 
    // 父进程代码块 (fork() 返回值大于 0)
    else {
        printf("--- 父进程开始执行 ---\n");
        printf("父进程读取到的初始 shared 值: %d\n", shared);

        // 循环对 shared 变量进行加法操作
        for (int i = 0; i < 5; i++) {
            shared += 10;
            printf("父进程: shared = %d\n", shared);
            sleep(1); // 暂停1秒，以便观察交替执行
        }
        
        // 等待子进程结束，回收其资源
        wait(NULL); 
        printf("--- 父进程执行结束 ---\n");
    }

    return 0;
}