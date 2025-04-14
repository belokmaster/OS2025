#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sched.h>
#include <stdlib.h>
#include <signal.h>

#include <sys/prctl.h>
#include <sys/wait.h>

// Программа создает два дочерних процесса с различными политиками планирования: первый использует FIFO (с приоритетом 41), 
// а второй — Round Robin (с тем же приоритетом). Каждый процесс выполняет бесконечный цикл, выводя символы 'F' и 'S'
// соответственно, чтобы продемонстрировать их выполнение. Программа также выводит информацию о процессах с помощью команды 
// ps и ожидает завершения дочерних процессов.

// FIFO — это неблокирующий механизм планирования, и процессы с этой политикой не могут быть прерваны, пока не завершат свой квант времени.

// Дочерний процесс 1, использующий политику FIFO
int clone1_routine(void *arg) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset); // Привязка к процессору 0

    // Устанавливаем политику FIFO с приоритетом 41 (такой же, как у дочернего процесса 2)
    sched_setscheduler(0, SCHED_FIFO, &(struct sched_param){.sched_priority=41});

    setbuf(stdout, NULL); // Отключение буферизации вывода
    prctl(PR_SET_NAME, "child1"); // Установка имени процесса

    // Бесконечный цикл, который выводит 'F' для демонстрации выполнения процесса
    while (1) {
        printf("F");
        fflush(stdout);
        usleep(1000); // Сон на 1 миллисекунду
    }

    return 0;
}

// Дочерний процесс 2, использующий политику Round Robin
int clone2_routine(void *arg) {
    // Устанавливаем политику Round Robin с приоритетом 41 (такой же, как у дочернего процесса 1)
    sched_setscheduler(0, SCHED_RR, &(struct sched_param){.sched_priority=41});

    setbuf(stdout, NULL); // Отключение буферизации вывода
    prctl(PR_SET_NAME, "child2"); // Установка имени процесса

    // Бесконечный цикл, который выводит 'S' для демонстрации выполнения процесса
    while (1) {
        printf("S");
        fflush(stdout);
        usleep(1000); // Сон на 1 миллисекунду
    }

    return 0;
}

int main(int argc, char* argv[]) {
    setbuf(stdout, NULL); // Отключение буферизации вывода для главного процесса

    // Выделение памяти для стека первого дочернего процесса
    char* stack1 = malloc(1024 * 1024);
    // Создание первого дочернего процесса (с политикой FIFO)
    pid_t child1_pid = clone(clone1_routine, stack1 + 1024 * 1024, SIGCHLD, NULL);

    // Выделение памяти для стека второго дочернего процесса
    char* stack2 = malloc(1024 * 1024);
    // Создание второго дочернего процесса (с политикой Round Robin)
    pid_t child2_pid = clone(clone2_routine, stack2 + 1024 * 1024, SIGCHLD, NULL);

    // Вывод информации о процессах (политика, приоритет и другие параметры)
    system("ps -o comm,policy,pri,ni,rtprio");

    // Ожидание завершения обоих дочерних процессов
    waitpid(child1_pid, NULL, 0);
    waitpid(child2_pid, NULL, 0);

    // Освобождение выделенной памяти для стеков
    free(stack1);
    free(stack2);

    return 0;
}
