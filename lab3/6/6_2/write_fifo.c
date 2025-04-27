#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define FIFO_NAME "/tmp/myfifo"

int main() {
    // Создаем FIFO (именованный канал)
    if (mkfifo(FIFO_NAME, 0666) == -1) {
        perror("mkfifo");
        exit(1);
    }
    printf("FIFO %s created.\n", FIFO_NAME);

    // Открываем FIFO для записи
    int fd = open(FIFO_NAME, O_WRONLY);
    if (fd == -1) {
        perror("open");
        exit(1);
    }

    // Записываем сообщение в FIFO
    const char *message = "Hello from FIFO!";
    write(fd, message, sizeof(message));

    // Закрываем FIFO
    close(fd);
    return 0;
}
