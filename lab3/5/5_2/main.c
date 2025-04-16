#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define SHM_SIZE 1024
#define NUM_ITERATIONS 100000

// Структура для разделяемой памяти
struct shared_memory {
    char data[SHM_SIZE];
    int written;
};

// Глобальные переменные для каждого механизма
struct shared_memory shm_sem, shm_mutex, shm_cond;
sem_t sem_writer, sem_reader;
pthread_mutex_t mutex;
pthread_cond_t cond;
pthread_mutex_t cond_mutex;

// Функция для измерения времени
double get_time_diff(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

// --- Реализация с семафорами ---
void* writer_sem(void* arg) {
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        sem_wait(&sem_writer);
        snprintf(shm_sem.data, SHM_SIZE, "Данные %d", i);
        shm_sem.written = 1;
        sem_post(&sem_reader);
    }
    return NULL;
}

void* reader_sem(void* arg) {
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        sem_wait(&sem_reader);
        shm_sem.written = 0;
        sem_post(&sem_writer);
    }
    return NULL;
}

double test_semaphores() {
    struct timespec start, end;
    pthread_t writer_thread, reader_thread;

    // Инициализация семафоров
    sem_init(&sem_writer, 0, 1);
    sem_init(&sem_reader, 0, 0);
    shm_sem.written = 0;

    clock_gettime(CLOCK_MONOTONIC, &start);
    pthread_create(&writer_thread, NULL, writer_sem, NULL);
    pthread_create(&reader_thread, NULL, reader_sem, NULL);
    pthread_join(writer_thread, NULL);
    pthread_join(reader_thread, NULL);
    clock_gettime(CLOCK_MONOTONIC, &end);

    sem_destroy(&sem_writer);
    sem_destroy(&sem_reader);
    return get_time_diff(start, end);
}

// --- Реализация с мьютексами ---
void* writer_mutex(void* arg) {
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        pthread_mutex_lock(&mutex);
        while (shm_mutex.written) {
            pthread_mutex_unlock(&mutex);
            usleep(100);
            pthread_mutex_lock(&mutex);
        }
        snprintf(shm_mutex.data, SHM_SIZE, "Данные %d", i);
        shm_mutex.written = 1;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void* reader_mutex(void* arg) {
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        pthread_mutex_lock(&mutex);
        while (!shm_mutex.written) {
            pthread_mutex_unlock(&mutex);
            usleep(100);
            pthread_mutex_lock(&mutex);
        }
        shm_mutex.written = 0;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

double test_mutex() {
    struct timespec start, end;
    pthread_t writer_thread, reader_thread;

    // Инициализация мьютекса
    pthread_mutex_init(&mutex, NULL);
    shm_mutex.written = 0;

    clock_gettime(CLOCK_MONOTONIC, &start);
    pthread_create(&writer_thread, NULL, writer_mutex, NULL);
    pthread_create(&reader_thread, NULL, reader_mutex, NULL);
    pthread_join(writer_thread, NULL);
    pthread_join(reader_thread, NULL);
    clock_gettime(CLOCK_MONOTONIC, &end);

    pthread_mutex_destroy(&mutex);
    return get_time_diff(start, end);
}

// --- Реализация с условными переменными ---
void* writer_cond(void* arg) {
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        pthread_mutex_lock(&cond_mutex);
        while (shm_cond.written) {
            // ждём, пока reader не сбросит
            pthread_cond_wait(&cond, &cond_mutex);
        }
        snprintf(shm_cond.data, SHM_SIZE, "Данные %d", i);
        shm_cond.written = 1;
        pthread_cond_signal(&cond);  // будим reader
        pthread_mutex_unlock(&cond_mutex);
    }
    return NULL;
}

void* reader_cond(void* arg) {
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        pthread_mutex_lock(&cond_mutex);
        while (!shm_cond.written) {
            pthread_cond_wait(&cond, &cond_mutex);
        }
        // эмулируем чтение
        shm_cond.written = 0;
        pthread_cond_signal(&cond);  // будим writer
        pthread_mutex_unlock(&cond_mutex);
    }
    return NULL;
}

double test_cond() {
    struct timespec start, end;
    pthread_t writer_thread, reader_thread;

    // Инициализация условной переменной и мьютекса
    pthread_mutex_init(&cond_mutex, NULL);
    pthread_cond_init(&cond, NULL);
    shm_cond.written = 0;

    clock_gettime(CLOCK_MONOTONIC, &start);
    pthread_create(&writer_thread, NULL, writer_cond, NULL);
    pthread_create(&reader_thread, NULL, reader_cond, NULL);
    pthread_join(writer_thread, NULL);
    pthread_join(reader_thread, NULL);
    clock_gettime(CLOCK_MONOTONIC, &end);

    pthread_mutex_destroy(&cond_mutex);
    pthread_cond_destroy(&cond);
    return get_time_diff(start, end);
}

int main() {
    double time_sem, time_mutex, time_cond;

    printf("Тестирование механизмов синхронизации (%d итераций)\n", NUM_ITERATIONS);
    printf("-----------------------------------------------\n");

    // Тестирование семафоров
    time_sem = test_semaphores();
    printf("Время выполнения (семафоры): %.6f секунд\n", time_sem);

    // Тестирование мьютексов
    time_mutex = test_mutex();
    printf("Время выполнения (мьютексы): %.6f секунд\n", time_mutex);

    // Тестирование условных переменных
    time_cond = test_cond();
    printf("Время выполнения (условные переменные): %.6f секунд\n", time_cond);

    // Сравнение
    printf("\nСравнение эффективности:\n");
    printf("Семафоры быстрее мьютексов на %.2f%%\n", 
           ((time_mutex - time_sem) / time_sem) * 100);
    printf("Семафоры быстрее условных переменных на %.2f%%\n", 
           ((time_cond - time_sem) / time_sem) * 100);
    printf("Мьютексы быстрее условных переменных на %.2f%%\n", 
           ((time_cond - time_mutex) / time_mutex) * 100);

    return 0;
}