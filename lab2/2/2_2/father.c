#include <stdio.h>      // printf, fprintf
#include <stdlib.h>     // exit, system, EXIT_FAILURE
#include <unistd.h>     // fork, execl, getpid, getppid, sleep
#include <sys/wait.h>   // waitpid, WIFEXITED, etc.
#include <sys/types.h>  // pid_t

int main(int argc, char* argv[]) {
    // Проверка аргументов
    if (argc != 2) {
        fprintf(stderr, "Usage: %s CHILD_EXECUTABLE_PATH\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    printf("Parent (PID=%d): Starting up.\n", getpid());

    // Сохраняем список процессов ДО fork
    printf("Parent: Saving process list before fork to 'before_fork.log'...\n");
    if (system("ps -l > before_fork.log") == -1) {
        perror("system() before fork failed");
    }

    pid_t cpid = fork();
    if (cpid == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (cpid == 0) {
        // --- Дочерний процесс ---
        printf("Child (PID=%d, PPID=%d): Executing '%s'...\n", getpid(), getppid(), argv[1]);
        execl(argv[1], argv[1], (char*)NULL);

        // Если execl не удался
        perror("Child: execl() failed");
        exit(EXIT_FAILURE);
    } else {
        // --- Родительский процесс ---
        printf("Parent (PID=%d): Created child with PID=%d.\n", getpid(), cpid);
        sleep(1); // Подождём немного, пока дочерний процесс стартует

        printf("Parent: Saving process list after fork to 'after_fork.log'...\n");
        if (system("ps -l > after_fork.log") == -1) {
            perror("system() after fork failed");
        }

        // Ждём завершения дочернего процесса
        int wstatus;
        printf("Parent: Waiting for child to finish...\n");
        if (waitpid(cpid, &wstatus, 0) == -1) {
            perror("waitpid() failed");
        } else if (WIFEXITED(wstatus)) {
            printf("Parent: Child exited normally with status %d.\n", WEXITSTATUS(wstatus));
        } else {
            printf("Parent: Child terminated abnormally.\n");
        }

        // Сохраняем список процессов после завершения потомка
        printf("Parent: Saving process list after wait to 'after_wait.log'...\n");
        if (system("ps -l > after_wait.log") == -1) {
            perror("system() after wait failed");
        }
    }

    printf("Parent (PID=%d): Done.\n", getpid());
    return 0;
}
