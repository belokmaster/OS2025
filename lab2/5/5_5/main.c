#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sched.h>
#include <stdlib.h>
#include <signal.h>

#include <sys/prctl.h>
#include <sys/wait.h>

// Child process 1, using FIFO scheduling policy
int clone1_routine(void *arg) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset); // Bind to CPU 0

    // Set FIFO scheduler with priority 41 (same as child2)
    sched_setscheduler(0, SCHED_FIFO, &(struct sched_param){.sched_priority=41});

    setbuf(stdout, NULL); // Disable output buffering
    prctl(PR_SET_NAME, "child1"); // Set process name

    // Continuous loop printing 'F' to demonstrate process execution
    while (1) {
        printf("F");
        fflush(stdout);
        usleep(1000); // Sleep for 1ms
    }

    return 0;
}

// Child process 2, using Round Robin scheduling policy
int clone2_routine(void *arg) {
    // Set Round Robin scheduler with priority 41 (same as child1)
    sched_setscheduler(0, SCHED_RR, &(struct sched_param){.sched_priority=41});

    setbuf(stdout, NULL); // Disable output buffering
    prctl(PR_SET_NAME, "child2"); // Set process name

    // Continuous loop printing 'S' to demonstrate process execution
    while (1) {
        printf("S");
        fflush(stdout);
        usleep(1000); // Sleep for 1ms
    }

    return 0;
}

int main(int argc, char* argv[]) {
    setbuf(stdout, NULL); // Disable output buffering for the main process

    // Allocate memory for stack of the first child process
    char* stack1 = malloc(1024 * 1024);
    // Create the first child process (FIFO policy)
    pid_t child1_pid = clone(clone1_routine, stack1 + 1024 * 1024, SIGCHLD, NULL);

    // Allocate memory for stack of the second child process
    char* stack2 = malloc(1024 * 1024);
    // Create the second child process (Round Robin policy)
    pid_t child2_pid = clone(clone2_routine, stack2 + 1024 * 1024, SIGCHLD, NULL);

    // Print process information (policy, priority, etc.)
    system("ps -o comm,policy,pri,ni,rtprio");

    // Wait for both child processes to finish
    waitpid(child1_pid, NULL, 0);
    waitpid(child2_pid, NULL, 0);

    // Free the allocated memory for the stacks
    free(stack1);
    free(stack2);

    return 0;
}
