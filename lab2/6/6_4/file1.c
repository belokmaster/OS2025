#define _GNU_SOURCE // Важно: должно быть в самом начале файла
#include <stdio.h>
#include <sched.h>
#include <unistd.h>
#include <signal.h> // Для SIGCHLD
#include <sys/mman.h>

int global = 0;

int child_func(void *arg) {
    printf("[Child] global = %d\n", global);
    global = 42;
    printf("[Child] Updated global = %d\n", global);
    return 0;
}

int main() {
    void *stack = mmap(NULL, 1024*1024, PROT_READ | PROT_WRITE, 
                       MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);

    int flags = CLONE_VM | SIGCHLD; // Теперь SIGCHLD распознается

    if (clone(child_func, stack + 1024*1024, flags, NULL) == -1) {
        perror("clone");
        return 1;
    }

    sleep(1);
    printf("[Parent] global = %d\n", global);
    return 0;
}
