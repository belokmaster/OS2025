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

/*
Программа выполняет несколько операций с потоками и процессами, в том числе создание потоков с использованием 
POSIX-API и создание дочерних процессов с помощью системного вызова clone(). 
Она также анализирует их выполнение, отображая информацию о каждом процессе и потоке, 
а также их распределение во времени и связанные с ними параметры. Далее я расскажу, что происходит на каждом этапе.
*/

#define STACK_SIZE (1024 * 1024)  // Размер стека для дочернего процесса

// Функция для вывода информации о процессе или потоке
void print_info(const char* entity) {
    printf("%s: PID=%d, PPID=%d, TID=%ld, PGID=%d, SID=%d\n",
           entity,  // Название сущности (Main, Thread, Clone child)
           getpid(),  // ID процесса
           getppid(),  // ID родительского процесса
           (long)syscall(SYS_gettid),  // ID потока
           getpgid(0),  // ID группы процессов
           getsid(0));  // ID сессии
}

// POSIX (Portable Operating System Interface for Unix) — это набор стандартов, разработанных для обеспечения совместимости 
// между операционными системами Unix-подобных платформ. 

// Функция для потока (POSIX)
void* thread_routine(void* arg) {
    prctl(PR_SET_NAME, (unsigned long)"pthread");  // Задаем имя потока
    print_info("Thread");  // Выводим информацию о потоке
    
    // Демонстрация наследования параметров CPU affinity
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);  // Инициализируем маску CPU
    sched_getaffinity(0, sizeof(cpu_set_t), &cpuset);  // Получаем affinity текущего процесса
    printf("Thread CPU affinity: ");
    for (int i = 0; i < CPU_SETSIZE; i++)  // Перебираем все возможные процессоры
        if (CPU_ISSET(i, &cpuset))  // Если процесс может быть выполнен на этом процессоре
            printf("%d ", i);  // Выводим номер процессора
    printf("\n");
    
    while (1) sleep(1);  // Бесконечный цикл, чтобы поток продолжал работать
    return NULL;
}

// Функция для дочернего процесса, созданного через clone()
int clone_routine(void* arg) {
    prctl(PR_SET_NAME, "clone_child");  // Задаем имя процесса
    print_info("Clone child");  // Выводим информацию о дочернем процессе
    
    // Изменяем группу процессов для демонстрации
    setpgid(0, 0);  // Устанавливаем процесс в свою группу
    printf("After setpgid: PGID=%d\n", getpgid(0));  // Выводим новый PGID
    
    while (1) sleep(5);  // Бесконечный цикл для дочернего процесса
    return 0;
}

int main(void) {
    print_info("Main process");  // Выводим информацию о главном процессе
    
    // Устанавливаем маску уведомлений для дочерних процессов
    sigset_t mask;
    sigemptyset(&mask);  // Очищаем маску сигналов
    sigaddset(&mask, SIGCHLD);  // Добавляем SIGCHLD в маску
    sigprocmask(SIG_BLOCK, &mask, NULL);  // Блокируем сигнал SIGCHLD
    
    // Создаем POSIX поток
    pthread_t th;
    printf("\nCreating POSIX thread...\n");
    if (pthread_create(&th, NULL, thread_routine, NULL)) {  // Создание потока
        perror("pthread_create");  
        return 1;
    }
    printf("Thread created successfully\n");
    
    // Отсоединяем поток (поток будет самостоятельно очищен при завершении)
    pthread_detach(th);
    
    // Создаем процесс через clone()
    char* pchild_stack = malloc(STACK_SIZE);  // Выделяем память для стека дочернего процесса
    if (!pchild_stack) {  
        perror("malloc");
        return 1;
    }
    
    // В отличие от обычного вызова fork(), clone() позволяет точнее контролировать, какие ресурсы будут разделяться между родительским и дочерним процессом. 
    // Здесь используются флаги CLONE_VM и CLONE_FS, которые указывают на то, что память и файловая система будут совместными между процессами.
    printf("\nCreating clone process...\n");
    pid_t child_pid = clone(clone_routine,  // Передаем функцию для выполнения в дочернем процессе
                           pchild_stack + STACK_SIZE,  // Указываем место начала стека (для stack growing вниз)
                           SIGCHLD | CLONE_VM | CLONE_FS,  
                           NULL);  // Нет аргументов для дочернего процесса
    if (child_pid == -1) {  
        perror("clone");
        free(pchild_stack);  // Освобождаем память
        return 1;
    }
    printf("Clone process created successfully (PID=%d)\n", child_pid);  // Выводим PID дочернего процесса
    
    // Выводим информацию о планировании процессов
    printf("\nScheduling information:\n");
    system("ps -o pid,tid,class,rtprio,ni,pri,psr,pcpu,stat,wchan:20,comm");
    
    // Основной цикл мониторинга
    printf("\nStarting monitoring...\n");
    int counter = 0;
    while (1) {
        printf("\n=== Monitoring iteration %d ===\n", ++counter);
        
        // Подробная информация о процессах
        system("ps -eLf | head -1");  // Заголовок
        char cmd[100];
        sprintf(cmd, "ps -eLf | grep -E '%d|%s' | grep -v grep", 
                getpid(), "pthread|clone_child");  // Фильтрация по PID
        system(cmd);
        
        // Информация о потоках
        printf("\nThreads info:\n");
        system("ps -T -p $$ -o tid,spid,psr,pcpu,stat,comm");
        
        // Информация о планировщике
        printf("\nCPU affinity:\n");
        system("taskset -p $$");  // Печатаем affinity текущего процесса
        
        sleep(5);  // Ждем 5 секунд перед следующим мониторингом
    }
    
    free(pchild_stack);  // Освобождаем память
    return 0;
}
