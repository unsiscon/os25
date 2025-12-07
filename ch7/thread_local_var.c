#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
void *subtract_thread(void *arg) {
    int *shared_ptr = (int *)arg;
    printf("--- 子线程开始执行 ---\n");
    for (int i = 0; i < 5; i++) {
        int temp = *shared_ptr;
        temp -= 10;
        sleep(1);            
        *shared_ptr = temp;   
        printf("子线程: shared = %d\n", *shared_ptr);
    }
    printf("--- 子线程执行结束 ---\n");
    return NULL;
}
int main() {
    pthread_t tid;
    int shared = 100;
    printf("程序开始时, 局部变量 shared 的初始值: %d\n\n", shared);
    int ret = pthread_create(&tid, NULL, subtract_thread, (void *)&shared);
    if (ret != 0) {
        printf("线程创建失败: %d\n", ret);
        exit(1);
    }
    printf("--- 主线程继续执行 ---\n");
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