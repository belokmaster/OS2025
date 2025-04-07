#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

// Этот код создает два процесса (родительский и дочерний), каждый из которых выполняет имитацию длительных вычислений в отдельном цикле, 
// используя fork() для порождения дочернего процесса, тем самым демонстрируя псевдораспараллеливание. 
// Родительский процесс ожидает завершения дочернего перед продолжением выполнения программы.

// В процессе выполнения код демонстрирует псевдораспараллеливание — оба процесса (родительский и дочерний) работают независимо друг от друга, 
// выполняя вычисления параллельно.

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

        // Имитация длительных вычислений в процессе-потомке
        printf("Child: Starting calculations...\n");
        for (int i = 0; i < 5; i++) {
            printf("Child progress: %d/5\n", i + 1);
            volatile unsigned long long j; // volatile, чтобы компилятор не удалил цикл
            for (j = 0; j < 200000000ULL; j++); // Внутренний цикл для нагрузки процессора
        }
        printf("Child: Calculations finished.\n");
    } else { // Процесс-родитель
        printf("Parent process is: PID = %d, PPID = %d, Child PID = %d\n", 
               getpid(), getppid(), pid);

        // Имитация длительных вычислений в процессе-родителе
        for (int i = 0; i < 5; i++) {
            printf("Parent progress is: %d/5\n", i + 1);
            volatile long j;
            for (j = 0; j < 200000000ULL; j++); // Внутренний цикл для нагрузки процессора
        }
        printf("Parent: Waiting for child\n");
        wait(NULL); // Ожидаем завершение процесса-потомка
        printf("Child process finished. Parent continued\n");
    }

    printf("Program finished (PID = %d)\n", getpid());

    return 0;
}
