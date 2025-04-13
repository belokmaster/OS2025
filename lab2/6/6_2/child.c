#include <stdio.h>
#include <sched.h>
#include <sys/resource.h>

int main() {
    int policy = sched_getscheduler(0);
    int nice = getpriority(PRIO_PROCESS, 0);
    printf("Child after exec: Policy = %d, Nice = %d\n", policy, nice);
    return 0;
}
