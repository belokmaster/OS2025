#define _GNU_SOURCE // Должно быть в самом начале
#include <stdio.h>
#include <sched.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>

int child_func(void *arg) {
    printf("[Child] PID: %d\n", getpid());
    sleep(2);
    return 0;
}

int main() {
    // Выделяем память под стек
    void *stack = mmap(
        NULL, 
        1024 * 1024, 
        PROT_READ | PROT_WRITE, // Исправлено: PROT_READ и PROT_WRITE объявлены в sys/mman.h
        MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, // MAP_* константы из sys/mman.h
        -1, 
        0
    );

    // Флаги: CLONE_NEWPID (новое пространство PID) + SIGCHLD
    int flags = CLONE_NEWPID | SIGCHLD;

    if (clone(child_func, stack + 1024 * 1024, flags, NULL) == -1) {
        perror("clone");
        return 1;
    }

    printf("[Parent] PID: %d\n", getpid());
    return 0;
}
