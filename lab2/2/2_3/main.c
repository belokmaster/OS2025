#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

// Этот код демонстрирует использование функций из семейства exec() для замены текущего процесса на выполнение различных команд 
// в дочерних процессах. В процессе выполняются команды ls, env, и ps с разными способами передачи аргументов и переменных 
// окружения, а также с поиском исполняемых файлов в системе.

// Демонстрация использования execl()
void demo_execl() {
    printf("\n1. execl(): executing 'ls -l'\n");
    // execl() выполняет команду, указав путь к исполняемому файлу и аргументы.
    // Первый аргумент - это путь к исполняемому файлу, второй аргумент - имя команды,
    // остальные - аргументы для команды.
    // В данном случае выполняем команду 'ls -l', которая отображает список файлов в текущем каталоге.
    execl("/bin/ls", "ls", "-l", NULL); ы
    perror("execl failed");
    exit(EXIT_FAILURE);      
}

// Демонстрация использования execv()
void demo_execv() {
    printf("\n2. execv(): executing 'ls -l -a' with array args\n");
    // execv() работает аналогично execl(), но аргументы передаются в виде массива.
    // Массив строк, где первый элемент - это имя команды, а остальные - её аргументы.
    // В данном случае выполняем команду 'ls -l -a', которая отображает все файлы (включая скрытые).
    char *args[] = {"ls", "-l", "-a", NULL};
    execv("/bin/ls", args);
    perror("execv failed");
    exit(EXIT_FAILURE);      
}

// Демонстрация использования execle()
void demo_execle() {
    printf("\n3. execle(): executing 'env' with custom environment\n");
    // execle() аналогична execv(), но также позволяет передать переменные окружения.
    // В данном случае мы создаём переменную окружения CUSTOM_VAR и устанавливаем локаль.
    char *env[] = {"CUSTOM_VAR=HELLO", "LANG=ru_RU.UTF-8", NULL};
    // Выполняем команду 'env', которая выводит все переменные окружения
    execle("/usr/bin/env", "env", NULL, env);
    perror("execle failed");
    exit(EXIT_FAILURE);      
}

// Демонстрация использования execve()
void demo_execve() {
    printf("\n4. execve(): executing 'env' with custom args and env\n");
    // execve() - самая универсальная функция. Она позволяет передавать как аргументы, так и переменные окружения.
    // Массив аргументов для команды 'env'
    char *args[] = {"env", "GREETING=WORLD", NULL};
    // Массив переменных окружения
    char *env[] = {"USER=test_user", "PATH=/usr/bin:/bin", NULL};
    // Выполняем команду 'env' с переданными аргументами и переменными окружения
    execve("/usr/bin/env", args, env);
    perror("execve failed");
    exit(EXIT_FAILURE);      
}

// Демонстрация использования execlp()
void demo_execlp() {
    printf("\n5. execlp(): executing 'ps aux' (searched in PATH)\n");
    // execlp() работает как execl(), но исполняемый файл ищется в директориях, указанных в переменной окружения PATH.
    // В данном случае выполняем команду 'ps aux', которая отображает список всех процессов в системе.
    execlp("ps", "ps", "aux", NULL);
    perror("execlp failed");
    exit(EXIT_FAILURE);      
}

// Демонстрация использования execvp()
void demo_execvp() {
    printf("\n6. execvp(): executing 'ps -ef' with array args (searched in PATH)\n");
    // execvp() работает как execv(), но также ищет исполняемый файл в директориях из PATH.
    // В данном случае выполняем команду 'ps -ef', которая также выводит информацию о процессах.
    char *args[] = {"ps", "-ef", NULL};
    execvp("ps", args);
    perror("execvp failed");
    exit(EXIT_FAILURE);      
}

int main() {
    printf("Demonstration of exec() family functions\n");
    
    // Массив для хранения PID дочерних процессов и статус завершения процессов
    int pids[6], status;
    
    // Цикл для создания процессов и выполнения каждой демонстрации в дочернем процессе
    for (int i = 0; i < 6; i++) {
        pids[i] = fork();  // Создаём дочерний процесс
        
        if (pids[i] == 0) {
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
    
    // Ожидаем завершения всех дочерних процессов и выводим их статус
    for (int i = 0; i < 6; i++) {
        waitpid(pids[i], &status, 0);
        // Выводим PID дочернего процесса и его статус завершения
        printf("Process %d finished with status %d\n", pids[i], WEXITSTATUS(status));
    }
    
    printf("\nAll demonstrations completed\n");
    return 0;
}
