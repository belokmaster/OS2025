#include <stdio.h>
#include <unistd.h>
#include <sched.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <sys/wait.h>

// Программа устанавливает для родительского процесса политику планирования SCHED_RR (Round Robin) с приоритетом 50 и 
// значение nice равным -10 с помощью функций sched_setscheduler и setpriority. Затем, с помощью fork(), создается дочерний 
// процесс, в котором выводятся текущие значения политики и nice. После этого родительский процесс заменяет образ дочернего
// процесса на новый с помощью execl, указывая на выполнение внешней программы child. Родитель ожидает завершения дочернего 
// процесса с помощью wait(), а затем программа завершает выполнение.

void print_scheduling_info(const char *name) {
    int policy = sched_getscheduler(0);
    int nice = getpriority(PRIO_PROCESS, 0);
    printf("%s: Policy = %d, Nice = %d\n", name, policy, nice);
}

int main() {
    // Устанавливаем политику и приоритет родителя
    struct sched_param param = {.sched_priority = 50}; // Для SCHED_RR/FIFO
    sched_setscheduler(0, SCHED_RR, &param); // Требует прав root
    setpriority(PRIO_PROCESS, 0, -10); // Устанавливаем nice = -10

    print_scheduling_info("Parent");

    pid_t pid = fork();
    if (pid == 0) {
        print_scheduling_info("Child (after fork)");

        // Заменяем образ процесса на child
        execl("./child", "./child", NULL);  // Здесь заменяем на "child"
        perror("exec failed");
        exit(1);
    }

    wait(NULL);  // Ожидаем завершения дочернего процесса
    sleep(5);
    return 0;
}
