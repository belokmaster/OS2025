#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

void situation_a() {
    pid_t pid = fork();
    if (pid == 0) {
        // Ситуация А: Дочерний процесс завершается нормально
        printf("[A] Child PID=%d started (PPID=%d)\n", getpid(), getppid());
        sleep(4);
        printf("[A] Child PID=%d exiting\n", getpid());
        exit(0);
    } else {
        wait(NULL); // Родитель ожидает завершения
        printf("[A] Parent: Child %d finished\n\n", pid);
    }
}

void situation_b() {
    pid_t pid = fork();
    if (pid == 0) {
        // Ситуация Б: Сын создает внука и завершается
        pid_t grandchild = fork();
        if (grandchild == 0) {
            printf("[B] Grandchild PID=%d (original PPID=%d)\n", getpid(), getppid());
            sleep(1); // Ждем, чтобы родитель (сын) завершился
            printf("[B] Grandchild PID=%d (new PPID=%d adopted by systemd)\n\n", getpid(), getppid());
            exit(0);
        } else {
            printf("[B] Son PID=%d created grandchild %d\n", getpid(), grandchild);
            exit(0); // Сын завершается, оставляя внука сиротой
        }
    } else {
        sleep(3); // Даем время сыну завершиться
    }
}

void situation_c() {
    pid_t pid = fork();
    if (pid == 0) {
        // Ситуация В: Дочерний процесс становится зомби
        printf("[C] Child PID=%d exiting immediately\n", getpid());
        exit(0);
    } else {
        printf("[C] Parent: Created child %d (zombie)\n", pid);
        printf("[C] Zombie process state:\n");
        system("ps -o pid,ppid,state,command"); // Печатает состояние в консоль
        sleep(1); // Даем время для фиксации состояния
    }
}

int main() {
    printf("Main Parent PID=%d\n", getpid());

    situation_a(); // Нормальное завершение
    situation_b(); // Смена родителя на init
    situation_c(); // Появление зомби

    // Фиксируем итоговое состояние
    sleep(10);
    printf("Final process state:\n");
    system("ps -o pid,ppid,state,command"); // Печатает итоговое состояние в консоль
    return 0;
}
