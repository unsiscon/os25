#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

/*
 * 功能: 演示不同线程拥有独立的栈空间和私有的局部变量
 * 描述:
 *   - 创建两个子线程。
 *   - 每个子线程的函数内部都定义一个名为 'x' 的局部变量。
 *   - 观察到一个线程对 'x' 的修改完全不影响另一个线程的 'x'。
 *   - 这证明了线程的栈是私有的。
 */

// 线程一的执行函数
void *thread_1_func(void *arg) {
    // 1. 在线程函数内部定义局部变量，它将存在于该线程的私有栈上
    int x = 10;
    printf("线程1: 开始执行, x 的初始值是 %d\n", x);
    for (int i = 0; i < 5; i++) {
        x += 1;
        printf("线程1: x = %d\n", x);
        sleep(1);
    }
    printf("线程1: 执行结束\n");
    return NULL;
}

// 线程二的执行函数
void *thread_2_func(void *arg) {
    // 2. 这里也定义一个同名的局部变量，但它与线程一的 'x' 是完全不同的变量
    int x = 100;
    printf("线程2: 开始执行, x 的初始值是 %d\n", x);
    for (int i = 0; i < 5; i++) {
        x += 10;
        printf("线程2: x = %d\n", x);
        sleep(1);
    }
    printf("线程2: 执行结束\n");
    return NULL;
}

int main() {
    pthread_t tid1, tid2;

    printf("主线程: 准备创建两个子线程\n\n");

    // 创建两个线程
    pthread_create(&tid1, NULL, thread_1_func, NULL);
    pthread_create(&tid2, NULL, thread_2_func, NULL);

    // 等待两个线程都结束
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    printf("\n主线程: 所有子线程已执行完毕\n");

    return 0;
}