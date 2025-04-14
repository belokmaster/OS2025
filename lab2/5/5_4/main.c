#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <pthread.h>
#include <signal.h>
#include <sys/syscall.h>
#include <time.h>

// Этот код демонстрирует использование политики планирования SCHED_RR (Round-Robin) для процессов и потоков в Linux. 
// В начале устанавливается приоритет для главного процесса с использованием sched_setscheduler с политикой SCHED_RR, 
// после чего измеряется ориентировочный квант времени планировщика с помощью функции measure_rr_quantum().

// В программе создаются два потока с использованием библиотеки pthread, которые выводят символы "A" и "B" в бесконечном цикле
// с использованием sched_yield() для добровольной передачи процессорного времени. Также создается дочерний процесс с 
// использованием системного вызова clone, который выводит символ "C". Главный процесс выводит символ "M" в своем бесконечном 
// цикле, а все процессы и потоки работают с использованием sched_yield() для равномерного распределения процессорного времени.

// Программа нацелена на демонстрацию работы с политикой SCHED_RR, приоритетами процессов и измерение времени, которое 
// каждый процесс получает для выполнения в рамках этой политики.

// Квант времени — это минимальный отрезок времени, который процесс или поток может использовать процессор в операционной системе, 
// когда используется планировщик с политикой Round-Robin (RR) или другие политики с квантованием времени.

// sched_yield() — это системный вызов в операционных системах Linux, который используется для добровольной отдачи процессорного времени текущим процессом или потоком. 
// Когда процесс вызывает sched_yield(), он сообщает планировщику, что готов уступить процессорное время другим процессам, которые находятся в очереди на выполнение.

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
