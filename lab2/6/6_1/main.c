#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>

// Программа открывает файл test.txt для записи и выполняет действия как в родительском, так и в дочернем процессе. 
// Родительский процесс записывает сообщение в файл, затем порождает дочерний процесс, который также записывает в файл 
// и пытается выполнить другую программу с помощью execl. После завершения дочернего процесса родитель читает содержимое 
// файла и выводит его на экран.

int main() {
    // Открываем файл
    int fd = open("test.txt", O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    // Родительский процесс записывает в файл
    const char *parent_msg = "Parent writes: Hello\n";
    write(fd, parent_msg, strlen(parent_msg));

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        // Дочерний процесс
        const char *child_msg = "Child writes: World\n";
        write(fd, child_msg, strlen(child_msg));

        // Запускаем новую программу
        execl("./child", "./child", NULL);
        
        // Сюда попадем только если exec() не удался
        perror("exec");
        return 1;
    } else {
        // Родительский процесс ждет завершения дочернего
        wait(NULL);

        // Перемещаем указатель в начало файла
        lseek(fd, 0, SEEK_SET);

        // Читаем и выводим содержимое файла
        char buffer[100];
        int bytes = read(fd, buffer, sizeof(buffer));
        printf("File content:\n%.*s", bytes, buffer);
        close(fd);
    }

    return 0;
}
