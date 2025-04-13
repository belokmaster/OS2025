#include <unistd.h>
#include <fcntl.h>

int main() {
    // Open the file "test.txt" with read-write permissions
    int fd = open("test.txt", O_RDWR);

    // Write the string to the file
    write(fd, "EXEC writes: Success\n", 20);

    // Close the file descriptor
    close(fd);

    return 0;
}
