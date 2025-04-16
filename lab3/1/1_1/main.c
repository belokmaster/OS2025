#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <errno.h>

/*
Код создает две нити: первый поток (thread1_func) с интервалом в 3 секунды отправляет сигнал SIGUSR1 второму 
потоку (thread2_func), который реагирует на сигнал, устанавливая флаг и выводя сообщение. Через 10 секунд первый поток
завершает второй с помощью pthread_cancel. Программа демонстрирует межпотоковое взаимодействие с использованием сигналов
и управление жизненным циклом потоков.
*/

#define INTERVAL_SEC 3  // Интервал между отправками сигнала SIGUSR1 в секундах
#define TERMINATE_AFTER_SEC 10  // Через сколько секунд завершить вторую нить (по истечении этого времени)

volatile sig_atomic_t flag = 0;  // Флаг активации в потоке 2

// Обработчик сигнала для второй нити
void sigusr1_handler(int signo) {
    if (signo == SIGUSR1) {
        // Когда поток 2 получает сигнал SIGUSR1, выводится текущее время
        time_t now;
        time(&now);
        printf("Поток 2 получил SIGUSR1 в %s", ctime(&now));  // Выводим время получения сигнала
        flag = 1;  // Устанавливаем флаг активации
    }
}

// Функция для первой нити
void *thread1_func(void *arg) {
    pthread_t t2 = *(pthread_t *)arg;  // Получаем дескриптор второй нити
    time_t start_time, current_time;
    
    time(&start_time);  // Запоминаем время начала работы первой нити
    printf("Поток 1 запущен (PID=%d, TID=%lu)\n", getpid(), (unsigned long)pthread_self());
    
    while (1) {
        time(&current_time);  // Получаем текущее время
        double elapsed = difftime(current_time, start_time);  // Вычисляем прошедшее время
        
        // Отправляем сигнал SIGUSR1 каждые INTERVAL_SEC секунд
        if ((int)elapsed % INTERVAL_SEC == 0 && elapsed > 0) {
            printf("Поток 1 отправляет SIGUSR1 потоку 2 (прошло %.0f сек)\n", elapsed);
            if (pthread_kill(t2, SIGUSR1) != 0) {
                perror("pthread_kill");  // Обработка ошибки отправки сигнала
                break;
            }
        }
        
        // Завершаем вторую нить через TERMINATE_AFTER_SEC секунд
        if (elapsed >= TERMINATE_AFTER_SEC) {
            printf("Поток 1 завершает поток 2 через %.0f сек\n", elapsed);
            if (pthread_cancel(t2) != 0) {
                perror("pthread_cancel");  // Обработка ошибки завершения потока
            } else {
                printf("Поток 2 успешно завершен\n");
            }
            break;
        }
        
        sleep(1);  // Пауза перед следующей проверкой времени
    }
    
    return NULL;
}

// Функция для второй нити
void *thread2_func(void *arg) {
    // Настройка обработчика сигнала SIGUSR1 для текущего потока
    struct sigaction sa;
    sa.sa_handler = sigusr1_handler;  // Указываем функцию-обработчик
    sigemptyset(&sa.sa_mask);  // Не блокировать другие сигналы
    sa.sa_flags = 0;
    
    // Регистрация обработчика сигнала SIGUSR1
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");  // Обработка ошибки при регистрации обработчика
        return NULL;
    }

    printf("Поток 2 запущен (PID=%d, TID=%lu)\n", getpid(), (unsigned long)pthread_self());
    
    while (1) {
        // Если флаг активирован (получен сигнал), выводим сообщение
        if (flag) {
            printf("Поток 2: получен флаг активации\n");
            flag = 0;  // Сбрасываем флаг
        }
        sleep(1);  // Ожидаем 1 секунду перед проверкой флага
    }
    
    return NULL;
}

int main() {
    pthread_t t1, t2;  // Дескрипторы для двух потоков
    
    printf("Основной процесс (PID=%d)\n", getpid());
    
    // Создаем вторую нить
    if (pthread_create(&t2, NULL, thread2_func, NULL) != 0) {
        perror("pthread_create");  // Обработка ошибки при создании потока
        exit(EXIT_FAILURE);
    }
    
    // Даем время второй нити инициализироваться
    sleep(1);
    
    // Создаем первую нить, передаем ей идентификатор второй нити
    if (pthread_create(&t1, NULL, thread1_func, &t2) != 0) {
        perror("pthread_create");  // Обработка ошибки при создании потока
        exit(EXIT_FAILURE);
    }
    
    // Ожидаем завершения обеих нитей
    pthread_join(t1, NULL);  // Ожидаем завершения первой нити
    pthread_join(t2, NULL);  // Ожидаем завершения второй нити
    
    printf("Программа завершена\n");
    return 0;
}
