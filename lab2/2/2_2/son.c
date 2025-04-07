#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    printf("Son:    PID=%d, PPID=%d\n", getpid(), getppid());
    system("ps -l > son_processes.log"); // Состояние процессов в потомке
    sleep(2);
    printf("Son: Exiting...\n");
    exit(EXIT_SUCCESS);
}
