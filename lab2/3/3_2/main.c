#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

#define NUM_CHILDREN 3

int main() {
    pid_t pids[NUM_CHILDREN];  // Массив для хранения PID'ов детей
    int exit_codes[NUM_CHILDREN] = {15, 25, 35};  // Коды завершения
    
    // Запускаем все дочерние процессы
    for (int i = 0; i < NUM_CHILDREN; i++) {
        pids[i] = fork();
        
        if (pids[i] < 0) {  // Ошибка fork()
            perror("Fork failed");
            exit(1);
        } else if (pids[i] == 0) {  // Код дочернего процесса
            printf("Процесс-потомок %d (PID=%d) запущен\n", i + 1, getpid());
            sleep(2 * (i + 1));  // Каждый спит разное время
            printf("Потомок %d завершается с кодом %d\n", i + 1, exit_codes[i]);
            exit(exit_codes[i]);
        }
    }

    // Родительский процесс
    printf("Родитель (PID=%d) ждёт второго потомка (PID=%d)...\n", getpid(), pids[1]);
    
    int status;
    pid_t finished_pid = waitpid(pids[1], &status, 0);  // Ждём только второго
    
    if (finished_pid == -1) {
        perror("Ошибка waitpid");
    } else if (WIFEXITED(status)) {
        printf("Потомок %d завершился с кодом: %d\n", 
               (finished_pid == pids[0] ? 1 : (finished_pid == pids[1] ? 2 : 3)), 
               WEXITSTATUS(status));
    }

    // Дождаться остальных, чтобы не было зомби
    while (wait(NULL) > 0);  // Цикл, пока есть завершённые дети
    
    printf("Родительский процесс завершён.\n");
    return 0;
}
