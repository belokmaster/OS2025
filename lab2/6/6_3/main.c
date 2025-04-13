#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

// Программа демонстрирует работу с сигналами в родительском процессе и его потомке. В родительском процессе устанавливается 
// обработчик для сигнала SIGINT и блокируется сигнал SIGTERM. Затем создается дочерний процесс, который перед выполнением 
// программы проверяет обработчик для сигнала SIGINT. Родительский процесс отправляет два сигнала: SIGINT 
// (который обрабатывается в дочернем процессе) и заблокированный SIGTERM, который не будет обработан дочерним процессом.

void handler(int sig) {
    printf("Handler in %d\n", getpid());
}

int main() {
    struct sigaction sa;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL); // Установка обработчика

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGTERM);
    sigprocmask(SIG_BLOCK, &mask, NULL); // Блокируем SIGTERM

    pid_t pid = fork();
    if (pid == 0) {
        printf("Child before exec:\n");
        printf("SIGINT handler: %p\n", signal(SIGINT, SIG_DFL)); // Проверка обработчика
        execl("./child", "./child", NULL);
        perror("exec");
        return 1;
    } else {
        sleep(1);
        kill(pid, SIGINT); // Отправляем SIGINT
        kill(pid, SIGTERM); // Отправляем SIGTERM (заблокирован)
        wait(NULL);
    }
    return 0;
}
