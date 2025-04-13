#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/resource.h>

// Команда nice используется в Linux для изменения приоритета процесса. 
// Она позволяет устанавливать так называемый "nice value", который влияет на приоритет процесса в планировщике. 
// В системе Linux чем ниже значение nice, тем выше приоритет. Для непривилегированных пользователей можно только увеличивать (снижать приоритет) значение nice, а уменьшить его можно только для привилегированных пользователей (например, для root).

// Системный вызов getpriority(2) позволяет получить приоритет процесса, используя его nice value. 
// Этот системный вызов возвращает текущий nice value процесса, а также его приоритет, который можно использовать для 
// мониторинга и анализа процессов.

/*
Этот код демонстрирует работу с приоритетами процессов в Linux. Он выводит начальный приоритет текущего процесса, 
пытается изменить его на более высокий (низкое значение приоритета), проверяет результат изменения и выводит приоритет 
системного процесса, например, init или systemd. Для изменения приоритета процесса требуется привилегия root, и программа 
учитывает эту особенность.
*/
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
