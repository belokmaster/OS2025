#include <stdio.h>    
#include <stdlib.h> 
#include <unistd.h>   
#include <sys/wait.h>   
#include <sys/types.h>  

// Этот код создаёт программу с двумя процессами: родительский процесс порождает дочерний через fork() и запускает его 
// с помощью execl(). Родитель записывает состояние таблицы процессов в файлы до и после создания дочернего процесса, 
// а также после его завершения, используя команду ps -l.

int main(int argc, char* argv[]) {
    // Проверка аргументов командной строки: требуется один аргумент (путь к исполнимому файлу дочернего процесса)
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
        // Дочерний процесс сообщает, что будет выполнять указанный исполнимый файл.
        printf("Child (PID=%d, PPID=%d): Executing '%s'...\n", getpid(), getppid(), argv[1]);

        // Дочерний процесс выполняет указанное приложение с помощью execl().
        execl(argv[1], argv[1], (char*)NULL);

        // Если execl не удался
        perror("Child: execl() failed");
        exit(EXIT_FAILURE);
    } else {
        // Родительский процесс
         // Родительский процесс сообщает, что он создал дочерний процесс.
        printf("Parent (PID=%d): Created child with PID=%d.\n", getpid(), cpid);
        sleep(1); // Подождём немного, пока дочерний процесс стартует

        printf("Parent: Saving process list after fork to 'after_fork.log'...\n");
        if (system("ps -l > after_fork.log") == -1) {
            perror("system() after fork failed");
        }

        // Родительский процесс ждёт завершения дочернего процесса.
        int wstatus;
        printf("Parent: Waiting for child to finish...\n");
        if (waitpid(cpid, &wstatus, 0) == -1) {  // Ожидаем завершения дочернего процесса.
            perror("waitpid() failed");  // Если произошла ошибка в waitpid, выводим ошибку.
        } else if (WIFEXITED(wstatus)) {  // Если дочерний процесс завершился нормально.
            printf("Parent: Child exited normally with status %d.\n", WEXITSTATUS(wstatus));  // Статус завершения дочернего процесса.
        } else {  // Если дочерний процесс завершился аномально.
            printf("Parent: Child terminated abnormally.\n");
        }

        // Сохраняем список процессов после завершения потомка
        printf("Parent: Saving process list after wait to 'after_wait.log'...\n");
        if (system("ps -l > after_wait.log") == -1) {
            perror("system() after wait failed");
        }
    }

    // Родительский процесс завершает выполнение.
    printf("Parent (PID=%d): Done.\n", getpid());
    return 0;
}
