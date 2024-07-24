#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include <stddef.h>  // For size_t (data type for sizes)

// Process States
typedef enum {
    PROCESS_STATE_NEW,        // Process just created
    PROCESS_STATE_READY,      // Ready to be executed
    PROCESS_STATE_RUNNING,    // Currently executing
    PROCESS_STATE_WAITING,    // Waiting for an event (e.g., I/O) 
    PROCESS_STATE_TERMINATED, // Finished execution
} ProcessState;

// Function Pointer Type for Entry Points (now takes optional data)
typedef void (*ProcessEntryPoint)(void* data);

// Process Creation Information Struct
typedef struct {
    ProcessEntryPoint entry_point; // Function to start the process
    void* data;                   // Optional data to pass to the entry point
    size_t stack_size;            // Stack size needed for the process (optional, default could be used)
} ProcessCreateInfo;

// Function Declarations

// Initialize the process manager
void process_init();

// Create a new process
//   - createInfo: Information about the process to create
// Returns:
//   - Process ID (PID) if successful, or -1 on failure
int create_process(const ProcessCreateInfo* createInfo);

// Schedule processes (e.g., round-robin scheduling)
void schedule_processes();

// Optional Functions (add as needed)

// Get the current process's PID
int get_current_process_id();

// Terminate a process
void terminate_process(int pid);

// Suspend a process (pause execution)
void suspend_process(int pid);

// Resume a suspended process
void resume_process(int pid);

// Get the state of a process
ProcessState get_process_state(int pid);

#endif // PROCESS_MANAGER_H
