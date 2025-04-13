#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sched.h>
#include <sys/wait.h>

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
