#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

/*
 * 功能: 演示通过传递指针来共享主线程的局部变量
 * 描述:
 *   - 在 main 函数内定义一个局部变量 shared。
 *   - 将该变量的地址作为参数传递给新创建的子线程。
 *   - 子线程通过指针来修改这个共享变量。
 *   - 结果与操作全局变量时完全一样，证明了线程共享的是整个进程的地址空间。
 */

// 线程要执行的函数
void *subtract_thread(void *arg) {
    // 1. 将传入的 void* 指针强制转换回 int* 指针
    int *shared_ptr = (int *)arg;

    printf("--- 子线程开始执行 ---\n");
    // 循环对 shared 变量进行减法操作
    for (int i = 0; i < 5; i++) {
        int temp = *shared_ptr; // 通过指针读取共享变量
        temp -= 10;
        sleep(1);               // 模拟耗时操作
        *shared_ptr = temp;     // 通过指针写回共享变量
        printf("子线程: shared = %d\n", *shared_ptr);
    }
    printf("--- 子线程执行结束 ---\n");
    return NULL;
}

int main() {
    pthread_t tid;

    // 2. 在 main() 函数内定义局部变量
    int shared = 100;

    printf("程序开始时, 局部变量 shared 的初始值: %d\n\n", shared);

    // 3. 创建线程，并将局部变量 shared 的地址 (&shared) 作为参数传入
    int ret = pthread_create(&tid, NULL, subtract_thread, (void *)&shared);
    if (ret != 0) {
        printf("线程创建失败: %d\n", ret);
        exit(1);
    }
    
    printf("--- 主线程继续执行 ---\n");
    // 主线程直接操作自己的局部变量
    for (int i = 0; i < 5; i++) {
        int temp = shared;
        temp += 10;
        sleep(1);
        shared = temp;
        printf("主线程: shared = %d\n", shared);
    }

    pthread_join(tid, NULL);

    printf("\n--- 所有线程执行完毕 ---\n");
    printf("最终 shared 的值是: %d\n", shared);

    return 0;
}