// son1.c - процесс с реакцией по умолчанию
#include <stdio.h>
#include <unistd.h>

int main() {
    printf("Son1 (PID: %d) запущен - реакция по умолчанию\n", getpid());
    while(1) {
        printf("Son1 работает...\n");
        sleep(1);
    }
    return 0;
}
