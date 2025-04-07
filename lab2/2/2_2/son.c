#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

// Этот код представляет программу для дочернего процесса, который выводит свой PID и PPID, затем сохраняет состояние 
// таблицы процессов в файл son_processes.log с помощью команды ps -l. После этого дочерний процесс делает паузу 
// на 2 секунды и завершает выполнение с успешным статусом.
int main() {
    printf("Son:    PID=%d, PPID=%d\n", getpid(), getppid());
    system("ps -l > son_processes.log"); // Состояние процессов в потомке
    sleep(2);
    printf("Son: Exiting...\n");
    exit(EXIT_SUCCESS);
}
