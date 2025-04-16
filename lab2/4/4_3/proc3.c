#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <syscall.h>
#include <sys/time.h>
#include <signal.h>

// Мьютекс для синхронизации вывода
pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;

// Структура для хранения времени сигнала
struct timeval signal_time[2];

// Функция для обработки сигналов и измерения времени
void signal_handler(int signum) {
    static int signal_count = 0;
    struct timeval current_time;
    gettimeofday(&current_time, NULL);  // Получаем текущее время

    pthread_mutex_lock(&print_mutex);
    printf("Thread [%ld] received signal: %d at time %ld.%06ld\n", syscall(SYS_gettid), signum,
           current_time.tv_sec, current_time.tv_usec);

    if (signal_count > 0) {
        long time_diff_sec = current_time.tv_sec - signal_time[signal_count - 1].tv_sec;
        long time_diff_usec = current_time.tv_usec - signal_time[signal_count - 1].tv_usec;
        if (time_diff_usec < 0) {
            time_diff_sec--;
            time_diff_usec += 1000000;
        }

        printf("Time difference between signals: %ld seconds and %ld microseconds\n",
               time_diff_sec, time_diff_usec);
    }

    signal_time[signal_count] = current_time;
    signal_count++;
    pthread_mutex_unlock(&print_mutex);
}

// Функция потока
void* thread_routine(void* arg) {
    int* fd = (int*)arg;
    
    pthread_mutex_lock(&print_mutex);
    printf("Thread [%ld]: Opening file with descriptor %d\n", syscall(SYS_gettid), *fd);
    pthread_mutex_unlock(&print_mutex);

    // Запись в файл
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), "Thread [%ld] writing to file\n", syscall(SYS_gettid));
    write(*fd, buffer, strlen(buffer));

    pthread_mutex_lock(&print_mutex);
    printf("Thread [%ld]: File written, now closing the file\n", syscall(SYS_gettid));
    pthread_mutex_unlock(&print_mutex);

    // Закрытие файла
    close(*fd);

    pthread_mutex_lock(&print_mutex);
    printf("Thread [%ld]: File closed\n", syscall(SYS_gettid));
    pthread_mutex_unlock(&print_mutex);

    // Попытка записи после закрытия файла
    if (write(*fd, "This should fail\n", 17) == -1) {
        pthread_mutex_lock(&print_mutex);
        printf("Thread [%ld]: Error: cannot write after file is closed\n", syscall(SYS_gettid));
        pthread_mutex_unlock(&print_mutex);
    }

    // Отправка сигнала главному потоку
    pthread_kill(pthread_self(), SIGUSR1);

    return NULL;
}

int main(void) {
    int ret = 0;

    // Установка обработчика сигналов
    signal(SIGUSR1, signal_handler);

    // Открытие файла
    int fd = open("shared.txt", O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if (fd == -1) {
        fprintf(stderr, "Error: open(): %s\n", strerror(errno));
        return 1;
    }

    // Создание двух потоков
    pthread_t thread1, thread2;
    printf("Creating threads...\n");
    
    if ((ret = pthread_create(&thread1, NULL, thread_routine, &fd)) != 0) {
        fprintf(stderr, "Error: pthread_create(): %s\n", strerror(ret));
        close(fd);
        return ret;
    }

    if ((ret = pthread_create(&thread2, NULL, thread_routine, &fd)) != 0) {
        fprintf(stderr, "Error: pthread_create(): %s\n", strerror(ret));
        close(fd);
        return ret;
    }

    // Ожидание завершения потоков
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    // Закрытие дескриптора файла
    pthread_mutex_lock(&print_mutex);
    printf("Main thread: File is already closed by threads\n");
    pthread_mutex_unlock(&print_mutex);

    return 0;
}
