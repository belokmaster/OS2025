#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define FIFO_NAME "/tmp/myfifo"

int main() {
    char buffer[128];

    // Открываем FIFO для чтения
    int fd = open(FIFO_NAME, O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(1);
    }

    // Читаем данные из FIFO
    read(fd, buffer, sizeof(buffer));
    printf("Received: %s\n", buffer);

    // Закрываем FIFO
    close(fd);
    return 0;
}
