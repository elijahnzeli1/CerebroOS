#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stddef.h>
#include <stdint.h> // for standard integer types

// Memory Allocation Strategies (for flexibility)
typedef enum {
    MEMORY_ALLOC_FIRST_FIT,  // Find the first available block
    MEMORY_ALLOC_BEST_FIT,    // Find the smallest suitable block
    // Add more strategies as needed (e.g., worst-fit, buddy allocation)
} MemoryAllocationStrategy;

// Memory Block Information (optional, for debugging/monitoring)
typedef struct {
    size_t size;        // Size of the allocated block
    void* address;      // Starting address of the allocated block
    // ... additional fields (e.g., allocation timestamp, usage statistics)
} MemoryBlockInfo;

// Function Declarations

// Initialize memory manager
//   - strategy: The allocation strategy to use
void memory_init(MemoryAllocationStrategy strategy = MEMORY_ALLOC_FIRST_FIT);

// Allocate a block of memory
//   - size: The size of the memory block to allocate
// Returns:
//   - A pointer to the allocated memory block, or NULL if allocation fails
void* memory_allocate(size_t size);

// Free a previously allocated block of memory
//   - ptr: A pointer to the memory block to free
void memory_free(void* ptr);

// Get information about a specific memory block (optional)
//   - ptr: A pointer to the memory block
// Returns:
//   - A MemoryBlockInfo structure containing information about the block, 
//     or NULL if the pointer is not valid.
MemoryBlockInfo* memory_get_block_info(void* ptr);

// Get a summary of memory usage (optional)
// Returns:
//   - A structure containing information about total memory, used memory,
//     number of allocated blocks, etc.
// struct MemoryUsageInfo memory_get_usage_info();

#endif // MEMORY_MANAGER_H
