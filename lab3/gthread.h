#ifndef GTHREAD_H
#define GTHREAD_H

#include <stdint.h>

#define MAX_THREADS 16   
#define STACK_SIZE (1024*64)
typedef enum {
    GTHREAD_READY,      
    GTHREAD_RUNNING,    
    GTHREAD_FINISHED    
} gthread_state_t;
typedef struct {
    uint64_t rsp;
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t rbx;
    uint64_t rbp;
} gthread_context_t;

typedef struct {
    int                 id;                
    gthread_state_t     state;              
    char*               stack;             
    gthread_context_t   context;           

    void                (*start_routine)(void*); 
    void*               arg;              
} gthread_tcb_t;

int gthread_create(int *tid, void (*start_routine)(void*), void *arg);

void gthread_init(void);

void gthread_exit(void);
int gthread_join(int tid);

#endif