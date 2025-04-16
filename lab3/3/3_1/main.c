#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

/*
Данный код реализует обработку сигналов с использованием функции sigaction() для настройки пользовательских обработчиков сигналов SIGINT, 
SIGTERM и SIGUSR1. Он также демонстрирует блокировку и разблокировку сигналов с помощью функции sigprocmask(), а также использование 
структуры sigaction для управления масками сигналов и флагами.
*/

// Обработчик для сигнала SIGINT (Ctrl+C)
void sigint_handler(int sig) {
    printf("Получен сигнал SIGINT (Ctrl+C)\n");
}

// Обработчик для сигнала SIGTERM
void sigterm_handler(int sig) {
    printf("Получен сигнал SIGTERM\n");
}

// Обработчик для сигнала SIGUSR1
void sigusr1_handler(int sig) {
    printf("Получен сигнал SIGUSR1\n");
}

int main() {
    struct sigaction newact, oldact;

    // Настройка обработчика для SIGINT
    newact.sa_handler = sigint_handler; // указывает функцию обработчика для сигнала SIGINT
    sigemptyset(&newact.sa_mask);  // Маска не блокирует другие сигналы
    newact.sa_flags = 0;  // флаг 0 означает, что никаких дополнительных флагов для обработчика не используется

    // Установка обработчика сигнала SIGINT
    if (sigaction(SIGINT, &newact, &oldact) == -1) {
        perror("Ошибка установки обработчика SIGINT");
        exit(EXIT_FAILURE);
    }

    // Настройка обработчика для сигнала SIGTERM
    newact.sa_handler = sigterm_handler; // Указание функции обработчика для сигнала SIGTERM
    sigemptyset(&newact.sa_mask);  // Маска не блокирует другие сигналы
    newact.sa_flags = 0;

    // Установка обработчика сигнала SIGTERM
    if (sigaction(SIGTERM, &newact, NULL) == -1) {
        perror("Ошибка установки обработчика SIGTERM");
        exit(EXIT_FAILURE);
    }

    // Настройка обработчика для сигнала SIGUSR1
    newact.sa_handler = sigusr1_handler; // Указание функции обработчика для сигнала SIGUSR1
    sigemptyset(&newact.sa_mask);  // Маска не блокирует другие сигналы
    newact.sa_flags = 0;

    // Установка обработчика сигнала SIGUSR1
    if (sigaction(SIGUSR1, &newact, NULL) == -1) {
        perror("Ошибка установки обработчика SIGUSR1");
        exit(EXIT_FAILURE);
    }

    // Маскирование сигналов в процессе
    sigset_t mask;
    sigemptyset(&mask);  // Инициализация пустой маски сигналов
    sigaddset(&mask, SIGINT);  // Добавляем сигнал SIGINT в маску для блокировки

    // Блокировка сигнала SIGINT
    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
        perror("Ошибка при маскировании сигнала SIGINT");
        exit(EXIT_FAILURE);
    }

    printf("Сигнал SIGINT заблокирован. Программа ожидает...\n");

    // Здесь сигнал SIGINT не будет обработан, так как он заблокирован
    sleep(5);  // Пауза для демонстрации блокировки сигнала

    // Разблокировка сигнала SIGINT
    if (sigprocmask(SIG_UNBLOCK, &mask, NULL) == -1) {
        perror("Ошибка при разблокировании сигнала SIGINT");
        exit(EXIT_FAILURE);
    }

    printf("Сигнал SIGINT теперь разблокирован. Ожидайте сигнала...\n");

    // Ждем 10 секунд для демонстрации реакции на сигналы
    sleep(10);

    printf("Завершение программы.\n");
    return 0;
}
