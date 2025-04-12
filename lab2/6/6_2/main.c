/*
 * Программа: sched_inherit_test.c
 * Назначение: Исследование наследования параметров планирования при fork() и exec().
 *             Демонстрирует:
 *             - Сохранение политик и приоритетов при создании потомка
 *             - Поведение nice-значений и параметров реального времени
 *             - Разницу между процессами до и после exec()
 *             Позволяет задавать произвольные параметры через командную строку.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <sys/wait.h>
#include <sys/resource.h>

// Вывод информации о параметрах планирования
void print_sched_info(const char *stage) {
    int policy = sched_getscheduler(0);  // Получаем текущую политику
    struct sched_param sp;
    sched_getparam(0, &sp);              // Получаем параметры планирования
    int nice = getpriority(PRIO_PROCESS, 0); // Получаем nice-значение

    printf("%s:\n  Policy: %s\n  Priority: %d\n  Nice: %d\n",
           stage,
           policy == SCHED_OTHER ? "OTHER" :  // CFS (стандартная политика)
           policy == SCHED_FIFO ? "FIFO" :    // Реальное время (FIFO)
           policy == SCHED_RR ? "RR" : "?",   // Реальное время (RR)
           sp.sched_priority,
           nice);
}

// Установка параметров планирования
void set_scheduling(int policy, int priority, int nice) {
    struct sched_param sp = {.sched_priority = priority};
    if (sched_setscheduler(0, policy, &sp))  // Устанавливаем политику и приоритет
        perror("Error setting scheduler");

    if (setpriority(PRIO_PROCESS, 0, nice))  // Устанавливаем nice-значение
        perror("Error setting nice");
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <policy> <priority> <nice>\n", argv[0]);
        return 1;
    }

    int policy = atoi(argv[1]);    // 0=OTHER, 1=FIFO, 2=RR
    int priority = atoi(argv[2]);  // Приоритет (1-99 для RT)
    int nice = atoi(argv[3]);      // Nice (-20..19 для OTHER)

    set_scheduling(policy, priority, nice);  // Настройка родителя
    print_sched_info("Parent before fork");

    pid_t pid = fork();  // Создание потомка
    if (pid == 0) {
        print_sched_info("Child after fork"); 
        execl("./sched_check", "./sched_check", NULL);  // Запуск проверочной утилиты
        perror("exec failed");
        exit(EXIT_FAILURE);
    }

    wait(NULL);  // Ожидание завершения потомка
    print_sched_info("Parent after wait");
    return 0;
}
