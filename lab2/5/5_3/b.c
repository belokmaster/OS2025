#include <stdio.h>
#include <sched.h>

// Этот код выводит диапазон приоритетов для планировщика SCHED_FIFO. Он использует функции sched_get_priority_min и 
// sched_get_priority_max для получения минимального и максимального значений приоритетов, которые могут быть установлены 
// для процессов, использующих политику планирования SCHED_FIFO. Затем результаты выводятся на экран, чтобы показать 
// допустимые границы приоритетов для этой политики.

int main() {
    int min = sched_get_priority_min(SCHED_FIFO);
    int max = sched_get_priority_max(SCHED_FIFO);
    printf("SCHED_FIFO priority range: %d to %d\n", min, max);
    return 0;
}
