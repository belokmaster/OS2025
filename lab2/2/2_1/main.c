#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid;

    // Создаём процесс-потомок
    pid = fork();

    if (pid < 0) { 
        perror("fork failed");
        exit(1);
    }

    if (pid == 0) { // Процесс-потомок
        printf("Child process: PID = %d, PPID = %d\n", getpid(), getppid());

        // Имитация длительных вычислений (цикл)
        printf("Child: Starting calculations...\n");
        for (int i = 0; i < 5; i++) {
            printf("Child progress: %d/5\n", i + 1);
            // Внутренний цикл для загрузки CPU
            volatile unsigned long long j; // volatile, чтобы компилятор не удалил цикл
            for (j = 0; j < 200000000ULL; j++); 
        }
        printf("Child: Calculations finished.\n");
    } else { // Процесс-родитель
        printf("Parent process is: PID = %d, PPID = %d, Child PID = %d\n", 
               getpid(), getppid(), pid);

        // Длительные вычисления (цикл)
        for (int i = 0; i < 5; i++) {
            printf("Parent progress is: %d/5\n", i + 1);
            volatile long j;
            for (j = 0; j < 200000000ULL; j++);
        }
        printf("Parent: Waiting for child\n");
        wait(NULL); // Ожидание завершения потомка
        printf("Child process finished. Parent continued\n");
    }

    printf("Program finished (PID = %d)\n", getpid());

    return 0;
}
