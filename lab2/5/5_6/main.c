#define _GNU_SOURCE      
#include <stdio.h>
#include <stdlib.h>       
#include <string.h>   
#include <pthread.h>        
#include <sched.h>        
#include <unistd.h>         
#include <sys/syscall.h>   

// Программа создает основной поток и дочерний поток, затем выводит информацию о текущих параметрах планирования каждого потока. 
// Вначале проверяются параметры планирования основного потока, после чего создается новый дочерний поток, который выполняет 
// свою функцию, а основной поток параллельно выполняет простую задачу. Каждый поток выводит информацию о своей политике 
// планирования и приоритете, используя pthread_getschedparam. В конце программа ожидает завершения дочернего потока с 
// помощью pthread_join.

// Функция для вывода информации о планировании ТЕКУЩЕГО потока
void display_current_thread_sched_info(const char* thread_name) {
    int policy;
    struct sched_param param;
    pthread_t self_tid = pthread_self(); // Получаем ID текущего потока

    // Получаем параметры планирования для текущего потока
    int ret = pthread_getschedparam(self_tid, &policy, &param);
    if (ret != 0) {
        fprintf(stderr, "[%s TID %lu]: Error getting sched params: %s\n",
                thread_name, (unsigned long)self_tid, strerror(ret));
        return;
    }

    // Определяем имя политики на основе стандартных POSIX значений
    const char* policy_name =
        (policy == SCHED_OTHER) ? "SCHED_OTHER (Normal/Default)" :
        (policy == SCHED_FIFO) ? "SCHED_FIFO (Realtime)" :
        (policy == SCHED_RR) ? "SCHED_RR (Realtime)" :
        "Unknown/Other";

    printf("[%s TID %lu, LWP %d]: Policy=%s, Static Priority=%d\n",
           thread_name, (unsigned long)self_tid, (int)syscall(SYS_gettid),
           policy_name, param.sched_priority);
}

// Функция, выполняемая новым потоком
void* child_thread_func(void* arg) {
    printf(" -> Child thread started.\n");

    // Определяем и выводим параметры планирования ЭТОГО потока
    display_current_thread_sched_info("Child");

    printf(" -> Child thread running simple loop...\n");
    for (int i = 0; i < 5; ++i) {
        printf(" C");
        fflush(stdout); // Для немедленного вывода
        usleep(150000); // 150 мс
    }
    printf("\n -> Child thread finished.\n");
    return NULL;
}

int main() {
    pthread_t child_tid;
    int ret;

    printf("[Main Thread] Starting program.\n");

    // 1. Определяем и выводим параметры планирования ОСНОВНОГО потока
    printf("[Main Thread] Checking its own scheduling parameters...\n");
    display_current_thread_sched_info("Main");

    // Устанавливаем небуферизованный вывод
    setvbuf(stdout, NULL, _IONBF, 0);

    // 2. Создаем новый поток
    printf("\n[Main Thread] Creating a child thread...\n");
    ret = pthread_create(&child_tid, NULL, child_thread_func, NULL);
    if (ret != 0) {
        fprintf(stderr, "[Main Thread] Error creating child thread: %s\n", strerror(ret));
        exit(EXIT_FAILURE);
    }

    // 3. Основной поток тоже немного работает, чтобы показать чередование
    printf("[Main Thread] Running simple loop...\n");
     for (int i = 0; i < 6; ++i) {
        printf(" M");
        usleep(120000); // 120 мс
    }
    printf("\n");


    // 4. Ожидаем завершения дочернего потока
    printf("[Main Thread] Waiting for child thread to join...\n");
    pthread_join(child_tid, NULL);
    printf("[Main Thread] Child thread joined.\n");

    printf("\n[Main Thread] Program finished.\n");
    return 0;
}
