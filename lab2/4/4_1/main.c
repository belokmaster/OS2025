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
// Поток засыпает на 1 секунду, чтобы продолжать работать бесконечно, не выполняя никакой реальной работы, но оставаться активным.
void* thread_routine(void* arg) {
    printf("Thread (pthread) PID=%d, TID=%ld\n", getpid(), syscall(SYS_gettid)); // Вывод PID и TID
    while (1) sleep(1); // Бесконечный сон — поток "живет", но ничего не делает
    return NULL;
}

// Функция, выполняемая clone-потоком
// Подобно предыдущей функции, она выводит PID и TID, затем засыпает, не выполняя других действий.
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
    // Первый и второй потоки начинают работать параллельно и выполняют бесконечный цикл с сном по 1 секунде.
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
    // Флаги, передаваемые в clone(), указывают, что новый поток будет совместно использовать с родительским процессом:
    // CLONE_VM: Совместное использование виртуальной памяти.
    // CLONE_FS: Совместное использование файловой системы.
    // CLONE_FILES: Совместное использование файловых дескрипторов.
    // CLONE_SIGHAND: Совместное использование обработчиков сигналов.
    // CLONE_THREAD: Поток будет частью текущего процесса (не создается новый процесс).

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
