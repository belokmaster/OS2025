#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/resource.h>

int main(void) {
    int pid = getpid();
    int initial_priority = getpriority(PRIO_PROCESS, pid);
    printf("Initial priority: %d\n", initial_priority);

    // Пытаемся повысить приоритет (для непривилегированных пользователей это может не сработать)
    int new_priority = -19;
    printf("Attempting to set priority to %d...\n", new_priority);
    
    if (setpriority(PRIO_PROCESS, pid, new_priority) == -1) {
        fprintf(stderr, "Error: setpriority(): %s\n", strerror(errno));
        if (errno == EPERM) {
            printf("Note: Only root can set negative nice values!\n");
        }
    } else {
        printf("Priority changed successfully.\n");
    }

    // Проверяем итоговый приоритет
    int final_priority = getpriority(PRIO_PROCESS, pid);
    printf("Final priority: %d\n", final_priority);

    // Сравнение с системными процессами (например, init)
    int system_process_pid = 1; // PID init/systemd
    int system_priority = getpriority(PRIO_PROCESS, system_process_pid);
    printf("System process (PID %d) priority: %d\n", system_process_pid, system_priority);

    return 0;
}
