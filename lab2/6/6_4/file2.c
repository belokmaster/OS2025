#define _GNU_SOURCE // Должно быть в самом начале
#include <stdio.h>
#include <sched.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>

// Программа использует системный вызов clone() для создания дочернего процесса в новом пространстве PID, 
// благодаря флагу CLONE_NEWPID. Дочерний процесс выводит свой PID, затем засыпает на 2 секунды. 
// Родительский процесс выводит свой PID после вызова clone. Для выделения памяти под стек дочернего процесса используется mmap()
// с флагами, указывающими на создание анонимного, приватного и стекового региона памяти.

int child_func(void *arg) {
    printf("[Child] PID: %d\n", getpid()); // Вывод PID дочернего процесса
    sleep(2); // Дочерний процесс засыпает на 2 секунды
    return 0;
}

int main() {
    // Выделяем память под стек для дочернего процесса
    void *stack = mmap(
        NULL, 
        1024 * 1024, 
        PROT_READ | PROT_WRITE, // Стек с правами на чтение и запись
        MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, // Создание анонимной памяти для стека
        -1, 
        0
    );

    // Флаги для clone: новое пространство PID и обработка сигнала SIGCHLD
    int flags = CLONE_NEWPID | SIGCHLD;

    if (clone(child_func, stack + 1024 * 1024, flags, NULL) == -1) {
        perror("clone"); // Ошибка при вызове clone
        return 1;
    }

    printf("[Parent] PID: %d\n", getpid()); // Вывод PID родительского процесса
    return 0;
}
