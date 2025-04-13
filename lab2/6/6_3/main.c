#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

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
