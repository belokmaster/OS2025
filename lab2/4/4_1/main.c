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

// Программа создает три потока: два с помощью pthread_create и один с использованием системного вызова clone, 
// имитируя поведение потоков с общим адресным пространством. Основной процесс периодически выводит информацию 
// о своих потоках через команду ps -T -p, отображая их PID и TID каждые 5 секунд.

#define STACK_SIZE (1024 * 1024) // Размер стека для clone-потока (1 МБ)

// Функция, выполняемая pthread-потоками
void* thread_routine(void* arg) {
    printf("Thread (pthread) PID=%d, TID=%ld\n", getpid(), syscall(SYS_gettid)); // Вывод PID и TID
    while (1) sleep(1); // Бесконечный сон — поток "живет", но ничего не делает
    return NULL;
}

// Функция, выполняемая clone-потоком
int clone_routine(void* arg) {
    printf("Thread (clone) PID=%d, TID=%ld\n", getpid(), syscall(SYS_gettid)); // Вывод PID и TID
    while (1) sleep(1); // Бесконечный сон
    return 0;
}

int main(void) {
    pthread_t thread1, thread2;
    char ps_command[256];

    // Создание двух потоков с использованием pthread_create
    pthread_create(&thread1, NULL, thread_routine, NULL);
    pthread_create(&thread2, NULL, thread_routine, NULL);

    // Выделение памяти под стек для clone-потока
    void *stack = malloc(STACK_SIZE);
    if (stack == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    // Флаги, обеспечивающие поведение clone-потока, аналогичное pthread
    int clone_flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD;

    // Создание потока с помощью clone()
    if (clone(clone_routine, stack + STACK_SIZE, clone_flags, NULL) == -1) {
        perror("clone");
        exit(EXIT_FAILURE);
    }

    // Вывод PID и TID основного потока (main)
    printf("Main process PID=%d, TID=%ld\n", getpid(), syscall(SYS_gettid));

    // Бесконечный цикл с выводом информации о потоках в процессе каждые 5 секунд
    while (1) {
        snprintf(ps_command, sizeof(ps_command), "ps -T -p %d", getpid()); // Команда для вывода потоков процесса
        system(ps_command); // Выполнение команды
        sleep(5); // Пауза перед следующим выводом
    }

    return 0;
}
