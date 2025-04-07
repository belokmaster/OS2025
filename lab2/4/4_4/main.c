#define _GNU_SOURCE
#include <stdio.h>
#include <sched.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <sys/wait.h>

pid_t tid;  // Для хранения идентификатора нити (TID)

// Функция для нити
void* thread_routine(void* arg)
{
    // Получение идентификатора нити
    tid = syscall(SYS_gettid);
    printf("Thread was started (TID=%d)!\n", tid);

    // Бесконечный цикл для демонстрации жизни нити
    while (1)
        sleep(1);

    return NULL;
}

// Функция для процесса, созданного с помощью clone
int clone_routine(void* arg)
{
    prctl(PR_SET_NAME, "child");
    printf("Child process (PID=%d) was started!\n", getpid());

    // Бесконечный цикл для демонстрации жизни процесса
    while (1)
        sleep(5);

    return 0;
}

int main(void)
{
    int ret = 0;
    pthread_t th;  // Указатель на нить
    char cmd_buf[255];  // Буфер для команд
    char n;

    // Создание нити
    printf("Creating thread...\n");
    if ((ret = pthread_create(&th, NULL, thread_routine, NULL)) != 0) {
        fprintf(stderr, "Error: pthread_create(): %s\n", strerror(ret));
        return ret;
    }
    sleep(1);  // Ожидание завершения инициализации
    printf("Thread was successfully created!\n");

    // Открепление нити
    printf("Detaching from thread...\n");
    if ((ret = pthread_detach(th)) != 0) {
        fprintf(stderr, "Error: pthread_detach(): %s\n", strerror(ret));
        return ret;
    }
    printf("Detach successful!\n");

    // Создание процесса с помощью clone
    char* pchild_stack = (char*)malloc(1024 * 1024);
    if (pchild_stack == NULL) {
        fprintf(stderr, "Error: malloc(): %s\n", strerror(errno));
        return errno;
    }

    printf("Creating child...\n");
    if ((ret = clone(clone_routine, (void*)(pchild_stack + 1024 * 1024), SIGCHLD, NULL)) == -1) {
        fprintf(stderr, "Error: clone(): %s\n", strerror(errno));
        free(pchild_stack);
        return 1;
    }
    sleep(1);  // Ожидание завершения инициализации
    printf("Child was created successfully (PID=%d)!\n", ret);

    printf("Start logging...\n");

    // До удаления
    printf("ps before kill:\n");
    system("ps -m -o state,pid,tid,sid,comm");

    // Удаление процесса
    snprintf(cmd_buf, sizeof(cmd_buf), "kill -9 %d", ret);
    system(cmd_buf);
    printf("ps after trying to kill child process (PID=%d):\n", ret);
    system("ps -m -o state,pid,tid,sid,comm");

    // Удаление нити
    snprintf(cmd_buf, sizeof(cmd_buf), "kill -9 %d", tid);
    system(cmd_buf);
    printf("ps after trying to kill thread (TID=%d):\n", tid);
    system("ps -m -o state,pid,tid,sid,comm");

    free(pchild_stack);  // Освобождение памяти

    return 0;
}
