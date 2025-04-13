#include <unistd.h>
#include <fcntl.h>

// Программа открывает файл test.txt для чтения и записи, записывает строку "EXEC writes: Success\n" в файл, 
// а затем закрывает файловый дескриптор.

int main() {
    // Открываем файл "test.txt" с правами чтения и записи
    int fd = open("test.txt", O_RDWR);

    // Записываем строку в файл
    write(fd, "EXEC writes: Success\n", 20);

    // Закрываем файловый дескриптор
    close(fd);

    return 0;
}
