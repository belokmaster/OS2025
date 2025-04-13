#include <stdio.h>
#include <sched.h>
#include <sys/resource.h>

// Программа извлекает и выводит текущую политику планирования и значение "nice" для процесса с использованием функций 
// sched_getscheduler и getpriority. Она выводит эти параметры для текущего процесса, чтобы продемонстрировать, как
// изменяются эти значения после выполнения команды exec.

int main() {
    int policy = sched_getscheduler(0);
    int nice = getpriority(PRIO_PROCESS, 0);
    printf("Child after exec: Policy = %d, Nice = %d\n", policy, nice);
    return 0;
}
