#include <stdio.h>
#include <unistd.h>
#include <signal.h>

// Этот код создает процесс, который игнорирует сигнал SIGTERM, что означает, что процесс не завершится 
// при получении этого сигнала. В бесконечном цикле процесс выводит сообщение о своей работе, продолжая 
// выполнение даже если будет послан сигнал SIGTERM.

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
