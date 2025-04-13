#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int main() {
    printf("Child after exec:\n");

    void *old_handler = signal(SIGINT, SIG_DFL); 

    printf("SIGINT handler: %p (SIG_DFL = 0x%lx)\n", old_handler, (unsigned long)SIG_DFL);

    while(1);

    return 0;
}
