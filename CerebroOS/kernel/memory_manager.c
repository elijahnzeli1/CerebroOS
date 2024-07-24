#include "memory_manager.h"
#include <stdint.h>
#include <stdbool.h>

#define MEMORY_SIZE 1048576 // 1 MB

// Memory Block Structure
typedef struct MemoryBlock {
    size_t size;    // Size of the block
    bool free;      // Indicates if the block is free
    struct MemoryBlock* next; // Pointer to the next block
} MemoryBlock;

// Head of the linked list of memory blocks
static MemoryBlock* memory_head = NULL;

// Initialize memory manager
void memory_init() {
    // Create a single free block encompassing all available memory
    memory_head = (MemoryBlock*)malloc(sizeof(MemoryBlock));
    if (!memory_head) {
        // Handle allocation failure (e.g., display an error message or halt)
        return;
    }
    memory_head->size = MEMORY_SIZE;
    memory_head->free = true;
    memory_head->next = NULL;
}

// Allocate memory
void* allocate_memory(size_t size) {
    // Find a suitable free block using a first-fit strategy
    MemoryBlock* current = memory_head;
    while (current != NULL) {
        if (current->free && current->size >= size) {
            // Split the block if it's larger than needed
            if (current->size > size + sizeof(MemoryBlock)) {
                MemoryBlock* newBlock = (MemoryBlock*)((uint8_t*)current + size + sizeof(MemoryBlock));
                newBlock->size = current->size - size - sizeof(MemoryBlock);
                newBlock->free = true;
                newBlock->next = current->next;
                current->next = newBlock;
            }
            current->size = size;
            current->free = false;
            return (uint8_t*)current + sizeof(MemoryBlock); // Return pointer after the header
        }
        current = current->next;
    }
    return NULL; // No suitable block found
}

// Free memory
void free_memory(void* ptr) {
    if (ptr == NULL) {
        return; // Ignore null pointers
    }

    // Get the memory block header
    MemoryBlock* block = (MemoryBlock*)((uint8_t*)ptr - sizeof(MemoryBlock));
    block->free = true;

    // Coalesce adjacent free blocks
    MemoryBlock* current = memory_head;
    while (current != NULL) {
        if (current->free && current->next && current->next->free) {
            current->size += current->next->size + sizeof(MemoryBlock);
            current->next = current->next->next;
            continue; // Check the new 'next' block
        }
        current = current->next;
    }
}
