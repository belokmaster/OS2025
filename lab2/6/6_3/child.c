#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int main() {
    // Print a message indicating that the child process is running after exec
    printf("Child after exec:\n");

    // Get the current handler for SIGINT (should be the default handler)
    void *old_handler = signal(SIGINT, SIG_DFL); // Use void* instead of sighandler_t

    // Print the current handler and SIG_DFL
    printf("SIGINT handler: %p (SIG_DFL = 0x%lx)\n", old_handler, (unsigned long)SIG_DFL);

    // Infinite loop to keep the child process running
    while(1);

    return 0;
}
