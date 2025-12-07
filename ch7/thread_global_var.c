#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
int shared = 100;
void *subtract_thread(void *arg) {
    printf("--- 子线程开始执行 ---\n");
    for (int i = 0; i < 5; i++) {
        int temp = shared;
        temp -= 10;
        sleep(1);          
        shared = temp;    
        printf("子线程: shared = %d\n", shared);
    }
    printf("--- 子线程执行结束 ---\n");
    return NULL;
}
int main() {
    pthread_t tid; 

    printf("程序开始时, 全局变量 shared 的初始值: %d\n\n", shared);
    int ret = pthread_create(&tid, NULL, subtract_thread, NULL);
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