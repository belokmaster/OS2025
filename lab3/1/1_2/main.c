#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <errno.h>

/*
Программа создает два потока, где первый поток отправляет сигнал SIGUSR1 второму потоку каждые 3 секунды. 
Второй поток реагирует на сигнал, выводя уведомление и завершает свою работу через обработчик сигнала с помощью pthread_exit. 
После 10 секунд работы первый поток завершает выполнение, и второй поток завершает свою работу 
самостоятельно при получении сигнала.
*/

/*
Отличия 1.1 от 1.2:
Механизм завершения потока:
-  В 1.1 второй поток завершался с помощью функции pthread_cancel, которая принудительно завершала поток извне.
-  В 1.2 второй поток завершает себя через pthread_exit(NULL) (ОБРАБОТЧИК СИГНАЛА), что позволяет потоку завершить свою работу 
самостоятельно после получения сигнала.

Обработчик сигнала:

-  В 1.1 второй поток просто устанавливает флаг при получении сигнала и продолжает работу.
-  В 1.2 второй поток использует обработчик сигнала, который не только устанавливает флаг, 
но и завершает поток с помощью pthread_exit(NULL).
*/

#define INTERVAL_SEC 3  // Интервал между сигналами в секундах
#define TERMINATE_AFTER_SEC 10  // Через сколько секунд завершить вторую нить

volatile sig_atomic_t flag = 0;

// Обработчик сигнала для второй нити (с добавлением уведомления и завершения через pthread_exit)
void sigusr1_handler(int signo) {
    if (signo == SIGUSR1) {
        time_t now;
        time(&now);
        printf("Поток 2 получил SIGUSR1 в %s", ctime(&now));
        printf("Обработчик сигнала начал свою работу.\n");
        pthread_exit(NULL);  // Завершаем второй поток, когда получен сигнал
    }
}

// Функция для второй нити
void *thread2_func(void *arg) {
    // Устанавливаем обработчик сигнала для этой нити
    struct sigaction sa;
    sa.sa_handler = sigusr1_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        return NULL;
    }

    printf("Поток 2 запущен (PID=%d, TID=%lu)\n", getpid(), (unsigned long)pthread_self());
    
    while (1) {
        sleep(1);  // Поток продолжает работать, ожидая сигнал
    }
    
    return NULL;
}

// Функция для первой нити
void *thread1_func(void *arg) {
    pthread_t t2 = *(pthread_t *)arg;
    time_t start_time, current_time;
    
    time(&start_time);
    printf("Поток 1 запущен (PID=%d, TID=%lu)\n", getpid(), (unsigned long)pthread_self());
    
    while (1) {
        time(&current_time);
        double elapsed = difftime(current_time, start_time);
        
        // Отправляем SIGUSR1 каждые INTERVAL_SEC секунд
        if ((int)elapsed % INTERVAL_SEC == 0 && elapsed > 0) {
            printf("Поток 1 отправляет SIGUSR1 потоку 2 (прошло %.0f сек)\n", elapsed);
            if (pthread_kill(t2, SIGUSR1) != 0) {
                perror("pthread_kill");
                break;
            }
        }
        
        // Завершаем вторую нить через TERMINATE_AFTER_SEC секунд
        if (elapsed >= TERMINATE_AFTER_SEC) {
            printf("Поток 1 завершает поток 2 через %.0f сек\n", elapsed);
            break;  // В этом случае второй поток завершится через обработчик сигнала
        }
        
        sleep(1);
    }
    
    return NULL;
}

int main() {
    pthread_t t1, t2;
    
    printf("Основной процесс (PID=%d)\n", getpid());
    
    // Создаем вторую нить
    if (pthread_create(&t2, NULL, thread2_func, NULL) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }
    
    // Даем время второй нити инициализироваться
    sleep(1);
    
    // Создаем первую нить, передаем ей идентификатор второй нити
    if (pthread_create(&t1, NULL, thread1_func, &t2) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }
    
    // Ожидаем завершения нитей
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    printf("Программа завершена\n");
    return 0;
}
