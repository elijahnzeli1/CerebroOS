#include "scheduler.h"
#include <stdio.h>

void init_scheduler() {
    printf("Scheduler initialized.\n");
}

void schedule_task(void (*task)()) {
    printf("Scheduling task...\n");
    task();
}
