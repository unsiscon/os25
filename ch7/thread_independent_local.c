#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
void *thread_1_func(void *arg) {
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
void *thread_2_func(void *arg) {
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
    pthread_create(&tid1, NULL, thread_1_func, NULL);
    pthread_create(&tid2, NULL, thread_2_func, NULL);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    printf("\n主线程: 所有子线程已执行完毕\n");
    return 0;
}