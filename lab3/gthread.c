#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include "gthread.h"

// 声明外部的汇编函数
extern void switch_context(gthread_context_t *old_ctx, gthread_context_t *new_ctx);

// 全局变量
static gthread_tcb_t g_thread_table[MAX_THREADS]; // 线程表
static int g_current_thread_id = 0;              // 当前运行的线程ID
static int g_next_tid = 1;                       // 下一个可用的线程ID

// 用于确保初始化只执行一次的静态变量
static int g_is_initialized = 0;


// 新增的线程启动函数
void gthread_bootstrap() {
    gthread_tcb_t *current_tcb = &g_thread_table[g_current_thread_id];
    
    // 调用真正的用户线程函数
    current_tcb->start_routine(current_tcb->arg);

    // 用户函数返回后，自动退出线程
    gthread_exit();
}


// 调度器函数
void schedule(int signum) {
    // 保存当前线程的上下文之前，先检查它是否已结束
    if (g_thread_table[g_current_thread_id].state == GTHREAD_RUNNING) {
        g_thread_table[g_current_thread_id].state = GTHREAD_READY;
    }

    // 轮转法寻找下一个就绪的线程
    int next_thread_id = (g_current_thread_id + 1) % MAX_THREADS;
    while (g_thread_table[next_thread_id].state != GTHREAD_READY) {
        next_thread_id = (next_thread_id + 1) % MAX_THREADS;
        // 防止死循环
        if (next_thread_id == g_current_thread_id) return;
    }
    
    int old_thread_id = g_current_thread_id;
    g_current_thread_id = next_thread_id;
    g_thread_table[g_current_thread_id].state = GTHREAD_RUNNING;

    printf("\n--- [Scheduler] Switching from thread %d to thread %d ---\n", old_thread_id, g_current_thread_id);

    // 调用汇编代码进行上下文切换
    switch_context(&g_thread_table[old_thread_id].context, &g_thread_table[g_current_thread_id].context);
}

// 线程函数的包装器，用于调用gthread_exit
void gthread_start_wrapper(void (*start_routine)(void), void *arg) {
    // 实际执行用户传入的线程函数 (这里简化了arg的使用)
    start_routine();
    // 线程函数执行完毕后，调用exit
    gthread_exit();
}


// gthread.c

// ...

// 初始化线程库
void gthread_init(void) {
    if (g_is_initialized) return;

    // 将所有TCB槽位标记为空闲 (通过state)
    for (int i = 0; i < MAX_THREADS; ++i) {
        g_thread_table[i].state = GTHREAD_FINISHED;
    }

    // 将主函数本身也看作一个线程 (线程0)
    g_thread_table[0].id = 0;
    g_thread_table[0].state = GTHREAD_RUNNING;
    g_current_thread_id = 0;

    // --- 修改开始 ---
    // 删除下面这行旧代码:
    // signal(SIGINT, schedule);

    // 使用 sigaction 来设置信号处理器，以获得更精确的控制
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa)); // 清空结构体
    sa.sa_handler = schedule;   // 指定处理函数为 schedule

    // sa_mask 定义了在处理器运行时额外要屏蔽的信号，我们置空它
    sigemptyset(&sa.sa_mask);

    // 关键！SA_NODEFER 标志告诉内核：
    // 在执行此信号处理器时，不要自动屏蔽触发它的那个信号。
    // 这就解决了我们遇到的问题！
    sa.sa_flags = SA_NODEFER; 

    // 注册信号处理器
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction failed");
        exit(1);
    }
    // --- 修改结束 ---

    g_is_initialized = 1;
}

// ...

// 创建一个新线程
// 重写 gthread_create 函数
int gthread_create(int *tid, void (*start_routine)(void*), void *arg) {
    if (!g_is_initialized) {
        gthread_init();
    }
    
    int new_tid = -1;
    for (int i = 1; i < MAX_THREADS; i++) {
        if (g_thread_table[i].state == GTHREAD_FINISHED) {
            new_tid = i;
            break;
        }
    }
    if (new_tid == -1) return -1;

    char *stack = (char*)malloc(STACK_SIZE);
    if (!stack) return -1;
    
    // 初始化TCB
    g_thread_table[new_tid].id = new_tid;
    g_thread_table[new_tid].state = GTHREAD_READY;
    g_thread_table[new_tid].stack = stack;
    g_thread_table[new_tid].start_routine = start_routine; // 保存函数和参数
    g_thread_table[new_tid].arg = arg;
    
    // --- 关键的上下文初始化 ---
    uint64_t *stack_top = (uint64_t*)(stack + STACK_SIZE);
    
    // 伪造返回地址，让 ret 跳转到我们的启动函数
    *(stack_top - 1) = (uint64_t)gthread_bootstrap;

    // 设置新线程的栈指针
    g_thread_table[new_tid].context.rsp = (uint64_t)(stack_top - 1);
    
    // 清空其他寄存器（好习惯）
    memset(&g_thread_table[new_tid].context, 0, sizeof(gthread_context_t));
    g_thread_table[new_tid].context.rsp = (uint64_t)(stack_top - 1);

    *tid = new_tid;
    return 0;
}


// 终止当前线程
void gthread_exit(void) {
    printf("\n--- Thread %d finished ---\n", g_current_thread_id);
    g_thread_table[g_current_thread_id].state = GTHREAD_FINISHED;
    // 释放栈空间
    free(g_thread_table[g_current_thread_id].stack);
    // 触发一次调度，切换到其他线程
    schedule(0);
}

// 等待一个线程结束
int gthread_join(int tid) {
    if (tid < 0 || tid >= MAX_THREADS) return -1;
    // 这是一个简单的忙等待实现
    while (g_thread_table[tid].state != GTHREAD_FINISHED) {
        // 主动让出CPU，触发调度（如果调度器是抢占式的）
        // 在我们的Ctrl+C模型下，这里会一直空转，直到用户按Ctrl+C
        // 这是一个设计上的简化
    }
    return 0;
}