// son3.c - процесс с перехватом сигнала
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

// Обработчик сигнала
void signal_handler(int sig) {
    printf("Son3 (PID: %d) поймал сигнал %d\n", getpid(), sig);
    printf("Son3 завершает работу по сигналу\n");
    exit(0);
}

int main() {
    // Устанавливаем обработчик для SIGUSR1
    signal(SIGUSR1, signal_handler);
    
    printf("Son3 (PID: %d) запущен - перехват сигнала\n", getpid());
    while(1) {
        printf("Son3 работает...\n");
        sleep(1);
    }
    return 0;
}
