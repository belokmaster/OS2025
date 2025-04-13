#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sched.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <sys/syscall.h>

// Программа демонстрирует различные политики планирования для процессов и потоков, а также влияние привязки к процессору 
// на выполнение этих сущностей. Она создает главный процесс, поток и дочерний процесс, все из которых привязываются к 
// первому процессору (CPU 0), и выводит информацию о планировании и привязке каждого из них. 
// Это позволяет наблюдать за их взаимодействием, производительностью и параметрами планирования.

// Структура для хранения информации о планировании
typedef struct {
    int policy;
    int priority;
    const char* policy_name;
} SchedInfo;

// Получение информации о планировании текущего контекста
SchedInfo get_scheduling_info() {
    SchedInfo info;
    struct sched_param param;
    
    info.policy = sched_getscheduler(0);
    sched_getparam(0, &param);
    info.priority = param.sched_priority;
    
    info.policy_name = (info.policy == SCHED_OTHER) ? "SCHED_OTHER" :
                      (info.policy == SCHED_FIFO)  ? "SCHED_FIFO" :
                      (info.policy == SCHED_RR)    ? "SCHED_RR" : "UNKNOWN";
    
    return info;
}

// Вывод информации о планировании
void print_scheduling_info(const char* context) {
    SchedInfo info = get_scheduling_info();
    printf("%s: policy = %s, priority = %d\n", 
           context, info.policy_name, info.priority);
}

// Привязка текущего контекста к указанному CPU
void bind_to_cpu(int cpu_id) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu_id, &cpuset);
    
    if (sched_setaffinity(0, sizeof(cpuset), &cpuset) == -1) {
        perror("sched_setaffinity failed");
        exit(EXIT_FAILURE);
    }
}

// Проверка привязки к CPU
int is_bound_to_cpu(int cpu_id) {
    cpu_set_t cpuset;
    if (sched_getaffinity(0, sizeof(cpuset), &cpuset) == -1) {
        perror("sched_getaffinity failed");
        return 0;
    }
    return CPU_ISSET(cpu_id, &cpuset);
}

// Функция потока
void* thread_func(void* arg) {
    bind_to_cpu(0);
    printf("Thread running on CPU %d\n", sched_getcpu());
    print_scheduling_info("Thread");
    
    while (1) {
        printf("T");
        fflush(stdout);
        usleep(10000);
    }
    return NULL;
}

// Функция дочернего процесса
int child_func(void* arg) {
    bind_to_cpu(0);
    prctl(PR_SET_NAME, "child_process");
    printf("Child running on CPU %d\n", sched_getcpu());
    print_scheduling_info("Child");
    
    while (1) {
        printf("C");
        fflush(stdout);
        usleep(10000);
    }
    return 0;
}

int main() {
    // Настройка unbuffered вывода для реального времени
    setvbuf(stdout, NULL, _IONBF, 0);
    bind_to_cpu(0);
    
    printf("Main process running on CPU %d\n", sched_getcpu());
    print_scheduling_info("Main (initial)");

    // Создание потока
    pthread_t thread;
    if (pthread_create(&thread, NULL, thread_func, NULL) != 0) {
        perror("pthread_create failed");
        exit(EXIT_FAILURE);
    }

    // Создание дочернего процесса через clone
    const int STACK_SIZE = 1024 * 1024;
    char* child_stack = malloc(STACK_SIZE);
    if (!child_stack) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    
    pid_t child_pid = clone(child_func, 
                           child_stack + STACK_SIZE, 
                           CLONE_VM | SIGCHLD, 
                           NULL);
    if (child_pid == -1) {
        perror("clone failed");
        free(child_stack);
        exit(EXIT_FAILURE);
    }

    sleep(1); // Даем время на инициализацию
    
    print_scheduling_info("Main (after creation)");

    while (1) {
        printf("M");
        fflush(stdout);
        usleep(10000);
    }

    free(child_stack);
    return 0;
}
