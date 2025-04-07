// father.c - родительский процесс
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

int main() {
    pid_t son1, son2, son3;

    // Сохраняем начальное состояние процессов
    system("ps -s > before.txt");

    // Создаем первого потомка
    son1 = fork();
    if (son1 == 0) {
        execl("./son1", "son1", NULL);
        exit(1);
    }

    // Создаем второго потомка
    son2 = fork();
    if (son2 == 0) {
        execl("./son2", "son2", NULL);
        exit(1);
    }

    // Создаем третьего потомка
    son3 = fork();
    if (son3 == 0) {
        execl("./son3", "son3", NULL);
        exit(1);
    }

    // Даем время потомкам настроить обработку сигналов
    sleep(1);

    // Отправляем сигнал SIGUSR1 всем потомкам
    printf("Отправка SIGUSR1 всем процессам...\n");
    kill(son1, SIGUSR1);
    kill(son2, SIGUSR1);
    kill(son3, SIGUSR1);

    // Ждем завершения всех потомков
    wait(NULL);
    wait(NULL);
    wait(NULL);

    // Сохраняем конечное состояние процессов
    system("ps -s > after.txt");

    printf("Программа завершена. Проверьте файлы before.txt и after.txt\n");
    return 0;
}
