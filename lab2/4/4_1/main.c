#define _GNU_SOURCE
#include <stdio.h>
#include <sched.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/syscall.h>
#include <sys/wait.h>

#define STACK_SIZE (1024 * 1024)

void* thread_routine(void* arg) {
    printf("Thread (pthread) PID=%d, TID=%ld\n", getpid(), syscall(SYS_gettid));
    while (1) sleep(1);
    return NULL;
}

int clone_routine(void* arg) {
    printf("Thread (clone) PID=%d, TID=%ld\n", getpid(), syscall(SYS_gettid));
    while (1) sleep(1);
    return 0;
}

int main(void) {
    pthread_t thread1, thread2;
    char ps_command[256];
    
    // Создаем два pthread потока
    pthread_create(&thread1, NULL, thread_routine, NULL);
    pthread_create(&thread2, NULL, thread_routine, NULL);
    
    // Создаем поток через clone()
    void *stack = malloc(STACK_SIZE);
    if (stack == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    
    int clone_flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD;
    if (clone(clone_routine, stack + STACK_SIZE, clone_flags, NULL) == -1) {
        perror("clone");
        exit(EXIT_FAILURE);
    }
    
    printf("Main process PID=%d, TID=%ld\n", getpid(), syscall(SYS_gettid));
    
    // Периодически выводим информацию о потоках
    while (1) {
        snprintf(ps_command, sizeof(ps_command), "ps -T -p %d", getpid());
        system(ps_command);
        sleep(5);
    }
    
    // В реальной программе нужно освобождать stack и завершать потоки
    // free(stack);
    return 0;
}
