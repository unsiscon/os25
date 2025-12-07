#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include "gthread.h"
extern void switch_context(gthread_context_t *old_ctx, gthread_context_t *new_ctx);
static gthread_tcb_t g_thread_table[MAX_THREADS]; 
static int g_current_thread_id = 0;             
static int g_next_tid = 1;                       
static int g_is_initialized = 0;
void gthread_bootstrap() {
    gthread_tcb_t *current_tcb = &g_thread_table[g_current_thread_id];
    current_tcb->start_routine(current_tcb->arg);
    gthread_exit();
}
void schedule(int signum) {
    if (g_thread_table[g_current_thread_id].state == GTHREAD_RUNNING) {
        g_thread_table[g_current_thread_id].state = GTHREAD_READY;
    }
    int next_thread_id = (g_current_thread_id + 1) % MAX_THREADS;
    while (g_thread_table[next_thread_id].state != GTHREAD_READY) {
        next_thread_id = (next_thread_id + 1) % MAX_THREADS;
        if (next_thread_id == g_current_thread_id) return;
    }
    int old_thread_id = g_current_thread_id;
    g_current_thread_id = next_thread_id;
    g_thread_table[g_current_thread_id].state = GTHREAD_RUNNING;
    printf("\n--- [Scheduler] Switching from thread %d to thread %d ---\n", old_thread_id, g_current_thread_id);
    switch_context(&g_thread_table[old_thread_id].context, &g_thread_table[g_current_thread_id].context);
}
void gthread_start_wrapper(void (*start_routine)(void), void *arg) {
    start_routine();
    gthread_exit();
}
void gthread_init(void) {
    if (g_is_initialized) return;
    for (int i = 0; i < MAX_THREADS; ++i) {
        g_thread_table[i].state = GTHREAD_FINISHED;
    }
    g_thread_table[0].id = 0;
    g_thread_table[0].state = GTHREAD_RUNNING;
    g_current_thread_id = 0;
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa)); 
    sa.sa_handler = schedule;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_NODEFER; 
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction failed");
        exit(1);
    }
    g_is_initialized = 1;
}
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
    g_thread_table[new_tid].id = new_tid;
    g_thread_table[new_tid].state = GTHREAD_READY;
    g_thread_table[new_tid].stack = stack;
    g_thread_table[new_tid].start_routine = start_routine; 
    g_thread_table[new_tid].arg = arg;
    uint64_t *stack_top = (uint64_t*)(stack + STACK_SIZE);
    *(stack_top - 1) = (uint64_t)gthread_bootstrap;
    g_thread_table[new_tid].context.rsp = (uint64_t)(stack_top - 1);
    memset(&g_thread_table[new_tid].context, 0, sizeof(gthread_context_t));
    g_thread_table[new_tid].context.rsp = (uint64_t)(stack_top - 1);
    *tid = new_tid;
    return 0;
}
void gthread_exit(void) {
    printf("\n--- Thread %d finished ---\n", g_current_thread_id);
    g_thread_table[g_current_thread_id].state = GTHREAD_FINISHED;
    free(g_thread_table[g_current_thread_id].stack);
    schedule(0);
}
int gthread_join(int tid) {
    if (tid < 0 || tid >= MAX_THREADS) return -1;
    while (g_thread_table[tid].state != GTHREAD_FINISHED) {
    }
    return 0;
}