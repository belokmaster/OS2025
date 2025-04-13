#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <pthread.h>
#include <signal.h>  // Добавлен для работы с сигналами
#include <sys/syscall.h>
#include <time.h>

// Функция для измерения кванта времени SCHED_RR
void measure_rr_quantum() {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    while (1) {
        clock_gettime(CLOCK_MONOTONIC, &end);
        long elapsed = (end.tv_sec - start.tv_sec) * 1000000 + 
                      (end.tv_nsec - start.tv_nsec) / 1000;
        if (elapsed > 1000) break;
    }
    printf("Estimated RR quantum: ~%ld μs\n", 
          (end.tv_sec - start.tv_sec) * 1000000 + 
          (end.tv_nsec - start.tv_nsec) / 1000);
}

void* thread_func(void* arg) {
    char symbol = *(char*)arg;
    while (1) {
        printf("%c", symbol);
        fflush(stdout);
        sched_yield();
    }
    return NULL;
}

int child_func(void* arg) {
    while (1) {
        printf("C");
        fflush(stdout);
        sched_yield();
    }
    return 0;
}

int main() {
    // Установка SCHED_RR для main
    struct sched_param param = {
        .sched_priority = sched_get_priority_max(SCHED_RR)
    };
    if (sched_setscheduler(0, SCHED_RR, &param) == -1) {
        perror("sched_setscheduler failed");
        exit(EXIT_FAILURE);
    }

    printf("Main process RR priority: %d\n", param.sched_priority);
    measure_rr_quantum();

    // Создание потоков
    pthread_t t1, t2;
    char sym1 = 'A', sym2 = 'B';
    pthread_create(&t1, NULL, thread_func, &sym1);
    pthread_create(&t2, NULL, thread_func, &sym2);

    // Создание дочернего процесса через clone()
    char* stack = malloc(1024 * 1024);
    if (!stack) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    pid_t pid = clone(child_func, stack + 1024 * 1024, 
                     CLONE_VM | SIGCHLD,  // Исправлено: SIGCHLD вместо SIGHLD
                     NULL);
    if (pid == -1) {
        perror("clone failed");
        free(stack);
        exit(EXIT_FAILURE);
    }

    // Главный цикл
    while (1) {
        printf("M");
        fflush(stdout);
        sched_yield();
    }

    free(stack);
    return 0;
}
