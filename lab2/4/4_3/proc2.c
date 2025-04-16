#include <stdio.h>
#include <unistd.h>

int global_var = 42;

int main() {
    pid_t pid = fork();

    if (pid == 0) {
        // do4
        global_var += 100;
        printf("Child: global_var = %d\n", global_var);
    } else {
        // rod
        sleep(1);  
        global_var += 1;
        printf("Parent: global_var = %d\n", global_var);
    }

    return 0;
}
