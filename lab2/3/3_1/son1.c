#include <stdio.h>
#include <unistd.h>

// Этот код создает процесс, который выводит сообщение о своем запуске и его PID, 
// а затем входит в бесконечный цикл, периодически выводя сообщение о своей работе. Процесс реагирует по умолчанию, без обработки сигналов или других событий.

int main() {
    printf("Son1 (PID: %d) запущен - реакция по умолчанию\n", getpid());
    while(1) {
        printf("Son1 работает...\n");
        sleep(1);
    }
    return 0;
}
