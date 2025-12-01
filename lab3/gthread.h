#ifndef GTHREAD_H
#define GTHREAD_H

#include <stdint.h> // 用于 uint64_t 等类型

#define MAX_THREADS 16      // 支持的最大线程数
#define STACK_SIZE (1024*64) // 每个线程的栈大小 (64KB)

// 线程状态
typedef enum {
    GTHREAD_READY,      // 就绪，可以被调度运行
    GTHREAD_RUNNING,    // 正在运行
    GTHREAD_FINISHED    // 已结束
} gthread_state_t;

// 线程上下文结构体 (对应 switch_context.S 中要保存的寄存器)
// x86-64 System V ABI规定，rbx, rbp, r12, r13, r14, r15 是被调用者保存的
// 我们还需要保存 rsp (栈指针) 和 rip (指令指针)
typedef struct {
    uint64_t rsp;
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t rbx;
    uint64_t rbp;
} gthread_context_t;

// 线程控制块 (TCB)
typedef struct {
    int                 id;                 // 线程ID
    gthread_state_t     state;              // 线程状态
    char*               stack;              // 指向栈底的指针
    gthread_context_t   context;            // 线程上下文

        // --- 新增字段 ---
    void                (*start_routine)(void*); // 线程的启动函数
    void*               arg;                // 传递给启动函数的参数
} gthread_tcb_t;

// 函数原型

// --- 修改函数原型 ---
// 现在 start_routine 接受一个 void* 参数
int gthread_create(int *tid, void (*start_routine)(void*), void *arg);

void gthread_init(void);

void gthread_exit(void);
int gthread_join(int tid);

#endif