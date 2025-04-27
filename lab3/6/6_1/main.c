#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main() {
    int pipefd[2]; // Массив для дескрипторов: pipefd[0] - чтение, pipefd[1] - запись
    pid_t pid;
    char buffer[100];

    // Создаём pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Делаем fork()
    pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { 
        // Дочерний процесс

        close(pipefd[1]); // Закрываем конец для записи

        // Читаем данные из канала
        read(pipefd[0], buffer, sizeof(buffer));
        printf("Child process received: %s\n", buffer);

        close(pipefd[0]); // Закрываем конец для чтения
    } else {
        // Родительский процесс

        close(pipefd[0]); // Закрываем конец для чтения

        char message[] = "Hello from parent!";
        // Пишем в канал
        write(pipefd[1], message, strlen(message) + 1);

        close(pipefd[1]); // Закрываем конец для записи
    }

    return 0;
}
