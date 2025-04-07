#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

// Программа иллюстрирует три ситуации с процессами: нормальное завершение дочернего процесса, создание "сироты" (внука), 
// оставленного родителем, и появление зомби-процесса. Для каждой ситуации выводятся сообщения о процессах и их состояниях, 
// а также фиксируется текущее состояние процессов с помощью команды ps.

// Сирота — это процесс, чьи родительские процессы завершились, и он был усыновлен системой (чаще всего процессом init или systemd), чтобы избежать утечек ресурсов. 
// Внук — это процесс, который является дочерним для другого дочернего процесса. 
// Зомби-процесс — это процесс, который завершился, но его родительский процесс еще не вызвал wait(), чтобы получить информацию о завершении. 
// В таком случае, процесс остаётся в таблице процессов как "зомби" до тех пор, пока родительский процесс не завершит его обработку.

// Ситуация A: Дочерний процесс завершает работу нормально
void situation_a() {
    pid_t pid = fork();
    if (pid == 0) {
        // Дочерний процесс
        printf("[A] Child PID=%d started (PPID=%d)\n", getpid(), getppid());
        sleep(4); // Даем время дочернему процессу для выполнения
        printf("[A] Child PID=%d exiting\n", getpid());
        exit(0); // Завершаем дочерний процесс
    } else {
        // Родительский процесс
        wait(NULL); // Ожидаем завершения дочернего процесса
        printf("[A] Parent: Child %d finished\n\n", pid);
    }
}

// Ситуация B: Сын создает внука и завершает, оставив его сиротой
void situation_b() {
    pid_t pid = fork();
    if (pid == 0) {
        // Сын создает внука
        pid_t grandchild = fork();
        if (grandchild == 0) {
            // Внук
            printf("[B] Grandchild PID=%d (original PPID=%d)\n", getpid(), getppid());
            sleep(1); // Ждем, чтобы родитель (сын) завершился
            printf("[B] Grandchild PID=%d (new PPID=%d adopted by systemd)\n\n", getpid(), getppid());
            exit(0); // Завершаем внука
        } else {
            // Сын
            printf("[B] Son PID=%d created grandchild %d\n", getpid(), grandchild);
            exit(0); // Сын завершает работу, оставляя внука сиротой
        }
    } else {
        sleep(3); // Даем время для завершения сына
    }
}

// Ситуация C: Дочерний процесс становится зомби
void situation_c() {
    pid_t pid = fork();
    if (pid == 0) {
        // Дочерний процесс
        printf("[C] Child PID=%d exiting immediately\n", getpid());
        exit(0); // Немедленное завершение дочернего процесса
    } else {
        // Родительский процесс
        printf("[C] Parent: Created child %d (zombie)\n", pid);
        printf("[C] Zombie process state:\n");
        system("ps -o pid,ppid,state,command"); // Печатает состояние в консоль
        sleep(1); // Даем время для фиксации состояния зомби
    }
}

int main() {
    printf("Main Parent PID=%d\n", getpid());

    situation_a(); // Нормальное завершение дочернего процесса
    situation_b(); // Смена родителя на init, оставление внука сиротой
    situation_c(); // Появление дочернего процесса-зомби

    sleep(10); // Даем время для завершения всех процессов
    printf("Final process state:\n");
    system("ps -o pid,ppid,state,command"); // Печатает итоговое состояние в консоль
    return 0;
}
