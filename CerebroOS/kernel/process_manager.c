#include "process_manager.h"
#include "memory_manager.h"
#include <stddef.h>  // for NULL

// Process States (to manage lifecycle)
typedef enum {
    PROCESS_STATE_NEW,
    PROCESS_STATE_READY,
    PROCESS_STATE_RUNNING,
    PROCESS_STATE_WAITING, // Add if you need blocking operations
    PROCESS_STATE_TERMINATED,
} ProcessState;

// Process Control Block (PCB)
typedef struct {
    int pid;                      // Process ID
    void (*entry_point)(void*);   // Function pointer (now takes void* for data)
    ProcessState state;           // Process state
    void* stack_pointer;          // Pointer to the process's stack
    // ... other process information (priority, context, etc.)
} ProcessControlBlock;

// Array to store PCBs
static ProcessControlBlock processes[MAX_PROCESSES];

// Pointer to the currently running process
static ProcessControlBlock* current_process = NULL;

// Initialize the process manager
void process_init() {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        processes[i].pid = -1;
        processes[i].state = PROCESS_STATE_NEW; // Mark all processes as new
        processes[i].stack_pointer = NULL;
    }
}

// Create a new process
int create_process(void (*entry_point)(void*), void* data) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].state == PROCESS_STATE_NEW) {
            processes[i].pid = i;
            processes[i].entry_point = entry_point;
            processes[i].state = PROCESS_STATE_READY;

            // Allocate stack for the process
            processes[i].stack_pointer = memory_allocate(STACK_SIZE); 
            if (!processes[i].stack_pointer) {
                return -1; // Stack allocation failed
            }
            
            // Initialize stack (e.g., set up arguments, return address, etc.)
            initialize_stack(processes[i].stack_pointer, entry_point, data);

            return i;
        }
    }
    return -1; // No available slots for new processes
}

// Round-Robin Scheduler (very basic)
void schedule_processes() {
    if (current_process && current_process->state == PROCESS_STATE_RUNNING) {
        current_process->state = PROCESS_STATE_READY;
    }

    do {
        current_process = &processes[(current_process ? current_process->pid + 1 : 0) % MAX_PROCESSES];
    } while (current_process->state != PROCESS_STATE_READY);

    current_process->state = PROCESS_STATE_RUNNING;
    switch_to_process(current_process->stack_pointer); 
}

// Current running process
static ProcessControlBlock* current_process = NULL;

// Initialize the process manager
void process_init() {
    // ... (same as before)

    // Initialize the timer for context switching (if using a timer-based approach)
    timer_init();
}

// Create a new process (as before)
int create_process(void (*entry_point)(void*), void* data) {
    // ... (same as before)
}

// Switch to a specific process (context switching)
void switch_to_process(void* stack_pointer) {
    // Save the current process's context (if there is one)
    if (current_process) {
        save_context(current_process->stack_pointer); 
    }

    // Update the current_process pointer
    current_process = (ProcessControlBlock*)((uint8_t*)stack_pointer - sizeof(ProcessControlBlock));

    // Restore the new process's context
    restore_context(stack_pointer);
}

// Round-Robin Scheduler (with timer interrupt)
void timer_interrupt_handler() { 
    schedule_processes(); // Trigger scheduling when timer interrupt occurs
}

void schedule_processes() {
    // ... (Round-robin scheduling logic from previous response)
}

// Terminate the current process
void terminate_process() {
    if (current_process) {
        current_process->state = PROCESS_STATE_TERMINATED;
        memory_free(current_process->stack_pointer); // Free the process's stack
        current_process = NULL; // No process running now

        // Find the next ready process to run
        schedule_processes();
    }
}

// ... (other process management functions)
