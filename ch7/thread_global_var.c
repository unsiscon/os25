#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

/*
 * 功能: 演示主线程和子线程并发操作同一个全局变量
 * 描述:
 *   - 定义一个全局变量 shared。
 *   - 创建一个新的子线程。
 *   - 主线程对 shared 进行循环加法操作。
 *   - 子线程对 shared 进行循环减法操作。
 *   - 观察 shared 变量值的混乱变化，理解线程间内存共享和竞态条件。
 */

// 1. 在 main() 函数外定义一个全局变量
int shared = 100;

// 线程要执行的函数
void *subtract_thread(void *arg) {
    printf("--- 子线程开始执行 ---\n");
    // 循环对 shared 变量进行减法操作
    for (int i = 0; i < 5; i++) {
        int temp = shared; // 读取共享变量
        temp -= 10;
        sleep(1);          // 模拟耗时操作，让线程切换更容易发生
        shared = temp;     // 写回共享变量
        printf("子线程: shared = %d\n", shared);
    }
    printf("--- 子线程执行结束 ---\n");
    return NULL;
}

int main() {
    pthread_t tid; // 用于存储线程ID

    printf("程序开始时, 全局变量 shared 的初始值: %d\n\n", shared);

    // 2. 创建一个新的线程，让它执行 subtract_thread 函数
    int ret = pthread_create(&tid, NULL, subtract_thread, NULL);
    if (ret != 0) {
        printf("线程创建失败: %d\n", ret);
        exit(1);
    }
    
    printf("--- 主线程继续执行 ---\n");
    // 主线程循环对 shared 变量进行加法操作
    for (int i = 0; i < 5; i++) {
        int temp = shared; // 读取共享变量
        temp += 10;
        sleep(1);          // 模拟耗时操作
        shared = temp;     // 写回共享变量
        printf("主线程: shared = %d\n", shared);
    }

    // 等待子线程执行结束，否则主线程可能先退出，导致整个进程结束
    pthread_join(tid, NULL);

    printf("\n--- 所有线程执行完毕 ---\n");
    printf("最终 shared 的值是: %d\n", shared);

    return 0;
}