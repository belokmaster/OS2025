// son2.c - процесс с игнорированием сигнала
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int main() {
    // Игнорируем сигнал SIGTERM
    signal(SIGTERM, SIG_IGN);
    
    printf("Son2 (PID: %d) запущен - игнорирование сигнала\n", getpid());
    while(1) {
        printf("Son2 работает...\n");
        sleep(1);
    }
    return 0;
}
