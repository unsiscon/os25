#include <stdio.h>
#include <unistd.h>
#include "gthread.h"

int thread_b_id, thread_c_id;
void thread_B_routine(void* arg) {
    while (1) {
        putchar('B');
        fflush(stdout); 
        sleep(1);
    }
}


void thread_C_routine(void* arg) {
    while (1) {
        putchar('C');
        fflush(stdout); 
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

    while (1) {
        putchar('A');
        fflush(stdout); 
        sleep(1);
    }
    return 0; 
}