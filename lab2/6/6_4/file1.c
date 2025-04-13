#define _GNU_SOURCE 
#include <stdio.h>
#include <sched.h>
#include <unistd.h>
#include <signal.h> 
#include <sys/mman.h>

// Программа создает новый процесс с помощью системного вызова clone(), который запускает функцию child_func. 
// В процессе родителя и дочернего процесса используется общий виртуальный адрес (на основе флага CLONE_VM), что позволяет 
// им разделять одну область памяти. Дочерний процесс изменяет глобальную переменную global, а родитель выводит её значение 
// после небольшого ожидания.

int global = 0;

// Функция, выполняемая дочерним процессом
int child_func(void *arg) {
    printf("[Child] global = %d\n", global); // Печать значения global в дочернем процессе
    global = 42; // Изменение глобальной переменной
    printf("[Child] Updated global = %d\n", global); // Печать нового значения global в дочернем процессе
    return 0;
}

int main() {
    // Выделение памяти для стека дочернего процесса
    void *stack = mmap(NULL, 1024*1024, PROT_READ | PROT_WRITE, 
                       MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);

    // Флаги для клонирования: общий адрес памяти и обработка сигнала SIGCHLD
    int flags = CLONE_VM | SIGCHLD;

    // Создание дочернего процесса с помощью clone()
    if (clone(child_func, stack + 1024*1024, flags, NULL) == -1) {
        perror("clone");
        return 1;
    }

    sleep(1); // Ожидание для того, чтобы дочерний процесс успел завершить выполнение
    printf("[Parent] global = %d\n", global); // Печать значения глобальной переменной в родительском процессе
    return 0;
}
