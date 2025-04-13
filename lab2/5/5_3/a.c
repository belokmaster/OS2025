#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sched.h>
#include <sys/wait.h>

/*
Этот код демонстрирует создание двух процессов, родительского и дочернего, с одинаковым приоритетом в планировщике 
с использованием политики SCHED_FIFO. Каждый процесс выводит буквы "A" и "B" соответственно в бесконечном цикле, при 
этом оба процесса используют одинаковый приоритет 50, установленный через sched_setscheduler. 
Это позволяет наблюдать, как процессы с одинаковым приоритетом взаимодействуют между собой в многозадачной системе.
*/

void set_fifo_priority(int priority) {
    struct sched_param param = {.sched_priority = priority};
    if (sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
        perror("sched_setscheduler");
        _exit(1);
    }
}

void worker(const char* name) {
    set_fifo_priority(50);  // Одинаковый приоритет
    while (1) {
        printf("%s", name);
        fflush(stdout);
        usleep(10000);
    }
}

int main() {
    pid_t pid = fork();
    if (pid == 0) {
        worker("A");  // Процесс-ребёнок
    } else {
        worker("B");  // Родительский процесс
    }
    return 0;
}
