#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sched.h>
#include <sys/wait.h>

// Этот код создает два процесса с разными приоритетами в рамках планировщика SCHED_FIFO. 
// Родительский процесс и дочерний процесс создаются с использованием функции fork(). 
// Дочерний процесс получает высокий приоритет (99), а родительский — низкий приоритет (1).
//  Оба процесса выполняют бесконечный цикл, выводя символы "H" и "L" соответственно, с использованием функции usleep() 
// для задержки между выводами. Важно, что приоритеты для процессов устанавливаются через системный вызов sched_setscheduler, 
// который меняет политику планирования для каждого процесса на SCHED_FIFO с заданным приоритетом.

void worker(const char* name, int priority) {
    struct sched_param param = {.sched_priority = priority};
    sched_setscheduler(0, SCHED_FIFO, &param);
    while (1) {
        printf("%s", name);
        fflush(stdout);
        usleep(10000);
    }
}

int main() {
    pid_t pid = fork();
    if (pid == 0) {
        worker("H", 99);  // Высокий приоритет
    } else {
        worker("L", 1);   // Низкий приоритет
    }
    return 0;
}
