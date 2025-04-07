#define _GNU_SOURCE
#include <stdio.h>
#include <sched.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/prctl.h>

#define STACK_SIZE (1024 * 1024)

void print_info(const char* entity) {
    printf("%s: PID=%d, PPID=%d, TID=%ld, PGID=%d, SID=%d\n",
           entity,
           getpid(),
           getppid(),
           (long)syscall(SYS_gettid),
           getpgid(0),
           getsid(0));
}

void* thread_routine(void* arg) {
    prctl(PR_SET_NAME, (unsigned long)"pthread");
    print_info("Thread");
    
    // Демонстрация наследования параметров CPU affinity
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    sched_getaffinity(0, sizeof(cpu_set_t), &cpuset);
    printf("Thread CPU affinity: ");
    for (int i = 0; i < CPU_SETSIZE; i++) 
        if (CPU_ISSET(i, &cpuset)) 
            printf("%d ", i);
    printf("\n");
    
    while (1) sleep(1);
    return NULL;
}

int clone_routine(void* arg) {
    prctl(PR_SET_NAME, "clone_child");
    print_info("Clone child");
    
    // Изменяем группу процессов для демонстрации
    setpgid(0, 0);
    printf("After setpgid: PGID=%d\n", getpgid(0));
    
    while (1) sleep(5);
    return 0;
}

int main(void) {
    print_info("Main process");
    
    // Устанавливаем маску уведомлений для дочерних процессов
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    sigprocmask(SIG_BLOCK, &mask, NULL);
    
    // Создаем POSIX поток
    pthread_t th;
    printf("\nCreating POSIX thread...\n");
    if (pthread_create(&th, NULL, thread_routine, NULL)) {
        perror("pthread_create");
        return 1;
    }
    printf("Thread created successfully\n");
    
    // Отсоединяем поток
    pthread_detach(th);
    
    // Создаем процесс через clone()
    char* pchild_stack = malloc(STACK_SIZE);
    if (!pchild_stack) {
        perror("malloc");
        return 1;
    }
    
    printf("\nCreating clone process...\n");
    pid_t child_pid = clone(clone_routine, 
                           pchild_stack + STACK_SIZE,
                           SIGCHLD | CLONE_VM | CLONE_FS,
                           NULL);
    if (child_pid == -1) {
        perror("clone");
        free(pchild_stack);
        return 1;
    }
    printf("Clone process created successfully (PID=%d)\n", child_pid);
    
    // Выводим информацию о планировании
    printf("\nScheduling information:\n");
    system("ps -o pid,tid,class,rtprio,ni,pri,psr,pcpu,stat,wchan:20,comm");
    
    // Основной цикл мониторинга
    printf("\nStarting monitoring...\n");
    int counter = 0;
    while (1) {
        printf("\n=== Monitoring iteration %d ===\n", ++counter);
        
        // Подробная информация о процессах
        system("ps -eLf | head -1");  // заголовок
        char cmd[100];
        sprintf(cmd, "ps -eLf | grep -E '%d|%s' | grep -v grep", 
                getpid(), "pthread|clone_child");
        system(cmd);
        
        // Информация о потоках
        printf("\nThreads info:\n");
        system("ps -T -p $$ -o tid,spid,psr,pcpu,stat,comm");
        
        // Информация о планировщике
        printf("\nCPU affinity:\n");
        system("taskset -p $$");
        
        sleep(5);
    }
    
    free(pchild_stack);
    return 0;
}
