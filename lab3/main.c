#include <stdio.h>
#include <unistd.h>
#include "gthread.h"

int thread_b_id, thread_c_id;

// 函数签名需要匹配，接收一个 void* 参数
void thread_B_routine(void* arg) {
    while (1) {
        putchar('B');
        fflush(stdout); // 使用fflush确保字符能立即输出
        sleep(1);
    }
}

// 函数签名需要匹配，接收一个 void* 参数
void thread_C_routine(void* arg) {
    while (1) {
        putchar('C');
        fflush(stdout); // 使用fflush确保字符能立即输出
        sleep(1);
    }
}

int main() {
    printf("Starting user-level thread demo.\n");
    printf("Press Ctrl+C to switch threads. Press Ctrl+Z to exit.\n");

    gthread_init(); 

    gthread_create(&thread_b_id, thread_B_routine, NULL);
    printf("Created thread B with id: %d\n", thread_b_id);

    gthread_create(&thread_c_id, thread_C_routine, NULL);
    printf("Created thread C with id: %d\n", thread_c_id);
    
    // --- 确保这部分代码存在！ ---
    // 这个循环是让主线程A保持运行的关键
    while (1) {
        putchar('A');
        fflush(stdout); // 使用fflush确保'A'能立即输出
        sleep(1);
    }
    // --- 确保这部分代码存在！ ---

    return 0; // 这行代码实际上永远不会被执行到
}