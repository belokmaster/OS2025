#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

void demo_execl() {
    printf("\n1. execl(): executing 'ls -l'\n");
    execl("/bin/ls", "ls", "-l", NULL);
    perror("execl failed");
    exit(EXIT_FAILURE);
}

void demo_execv() {
    printf("\n2. execv(): executing 'ls -l -a' with array args\n");
    char *args[] = {"ls", "-l", "-a", NULL};
    execv("/bin/ls", args);
    perror("execv failed");
    exit(EXIT_FAILURE);
}

void demo_execle() {
    printf("\n3. execle(): executing 'env' with custom environment\n");
    char *env[] = {"CUSTOM_VAR=HELLO", "LANG=ru_RU.UTF-8", NULL};
    execle("/usr/bin/env", "env", NULL, env);
    perror("execle failed");
    exit(EXIT_FAILURE);
}

void demo_execve() {
    printf("\n4. execve(): executing 'env' with custom args and env\n");
    char *args[] = {"env", "GREETING=WORLD", NULL};
    char *env[] = {"USER=test_user", "PATH=/usr/bin:/bin", NULL};
    execve("/usr/bin/env", args, env);
    perror("execve failed");
    exit(EXIT_FAILURE);
}

void demo_execlp() {
    printf("\n5. execlp(): executing 'ps aux' (searched in PATH)\n");
    execlp("ps", "ps", "aux", NULL);
    perror("execlp failed");
    exit(EXIT_FAILURE);
}

void demo_execvp() {
    printf("\n6. execvp(): executing 'ps -ef' with array args (searched in PATH)\n");
    char *args[] = {"ps", "-ef", NULL};
    execvp("ps", args);
    perror("execvp failed");
    exit(EXIT_FAILURE);
}

int main() {
    printf("=== Demonstration of exec() family functions ===\n");
    
    // Создаем процессы для каждого демо
    int pids[6], status;
    
    for (int i = 0; i < 6; i++) {
        pids[i] = fork();
        
        if (pids[i] == 0) { // Дочерний процесс
            switch(i) {
                case 0: demo_execl(); break;
                case 1: demo_execv(); break;
                case 2: demo_execle(); break;
                case 3: demo_execve(); break;
                case 4: demo_execlp(); break;
                case 5: demo_execvp(); break;
            }
        } else if (pids[i] < 0) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }
    }
    
    // Ожидаем завершения всех дочерних процессов
    for (int i = 0; i < 6; i++) {
        waitpid(pids[i], &status, 0);
        printf("Process %d finished with status %d\n", pids[i], WEXITSTATUS(status));
    }
    
    printf("\n=== All demonstrations completed ===\n");
    return 0;
}
