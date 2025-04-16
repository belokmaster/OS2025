#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <syscall.h>
#include <sys/wait.h>

pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;

int global_var1 = 5;   
int global_var2 = 100; 

void* thread_routine(void* arg) {
    pthread_mutex_lock(&print_mutex);

    global_var1 += 10;
    global_var2 -= 50;

    printf("Thread [%ld]: global_var1 = %d, global_var2 = %d\n",
           syscall(SYS_gettid), global_var1, global_var2);

    pthread_mutex_unlock(&print_mutex);
    return NULL;
}

int main(void) {
    pthread_t thread;

    printf("Creating thread...\n");
    if (pthread_create(&thread, NULL, thread_routine, NULL) != 0) {
        perror("pthread_create");
        return 1;
    }

    pthread_mutex_lock(&print_mutex);
    global_var1 += 20;
    global_var2 -= 25;

    printf("Main thread [%ld]: global_var1 = %d, global_var2 = %d\n",
           syscall(SYS_gettid), global_var1, global_var2);
    pthread_mutex_unlock(&print_mutex);

    pthread_join(thread, NULL);

    return 0;
}
