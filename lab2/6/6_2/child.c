/*
 * Программа: sched_check.c
 * Назначение: Вспомогательная утилита для проверки параметров планирования процесса.
 *             Выводит текущую политику планирования, приоритет и nice-значение.
 *             Используется для проверки наследования параметров после exec().
 */
#include <stdio.h>
#include <sched.h>
#include <sys/resource.h>

// Утилита для проверки параметров планирования после exec()
int main() {
    int policy = sched_getscheduler(0);  // Текущая политика
    struct sched_param sp;
    sched_getparam(0, &sp);              // Текущий приоритет
    int nice = getpriority(PRIO_PROCESS, 0); // Текущее nice-значение

    printf("Executed process:\n  Policy: %s\n  Priority: %d\n  Nice: %d\n",
           policy == SCHED_OTHER ? "OTHER" :
           policy == SCHED_FIFO ? "FIFO" :
           policy == SCHED_RR ? "RR" : "?",
           sp.sched_priority,
           nice);
    return 0;
}
