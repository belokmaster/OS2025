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

// Мьютекс для синхронизации вывода
pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;

int global_var = 5;  // Глобальная переменная для демонстрации общего состояния

// Функция для обработки сигналов
void signal_handler(int signum) {
    pthread_mutex_lock(&print_mutex);
    printf("Thread [%ld] received signal: %d\n", syscall(SYS_gettid), signum);
    pthread_mutex_unlock(&print_mutex);
}

// Функция нити
void* thread_routine(void* arg) {
    pthread_mutex_lock(&print_mutex);

    // Изменение глобальной переменной
    global_var += 10;
    printf("Thread [%ld]: Global var = %d\n", syscall(SYS_gettid), global_var);

    // Демонстрация общего доступа к файловым дескрипторам
    int fd = *((int*)arg);
    write(fd, "Hello from thread\n", 18);

    // Текущий рабочий каталог
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("Thread [%ld]: Current working directory = %s\n", syscall(SYS_gettid), cwd);

    pthread_mutex_unlock(&print_mutex);
    return NULL;
}

int main(void) {
    int ret = 0;

    // Установка обработчика сигналов
    signal(SIGUSR1, signal_handler);

    // Открытие файла для демонстрации общих файловых дескрипторов
    int fd = open("shared.txt", O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if (fd == -1) {
        fprintf(stderr, "Error: open(): %s\n", strerror(errno));
        return 1;
    }

    // Создание нити
    pthread_t thread;
    printf("Creating thread...\n");
    if ((ret = pthread_create(&thread, NULL, thread_routine, &fd)) != 0) {
        fprintf(stderr, "Error: pthread_create(): %s\n", strerror(ret));
        close(fd);
        return ret;
    }

    // Главная нить изменяет глобальную переменную
    pthread_mutex_lock(&print_mutex);
    global_var += 20;
    printf("Main thread [%ld]: Global var = %d\n", syscall(SYS_gettid), global_var);
    pthread_mutex_unlock(&print_mutex);

    // Главная нить отправляет сигнал дочерней нити
    pthread_kill(thread, SIGUSR1);

    // Ожидание завершения нити
    pthread_join(thread, NULL);

    // Закрытие дескриптора файла
    close(fd);

    return 0;
}
