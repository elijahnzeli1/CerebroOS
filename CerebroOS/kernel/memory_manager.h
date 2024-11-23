#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// Memory pool sizes for common allocations
#define TINY_BLOCK_SIZE 32
#define SMALL_BLOCK_SIZE 128
#define MEDIUM_BLOCK_SIZE 512

// Power efficiency modes
typedef enum {
    POWER_MODE_NORMAL,
    POWER_MODE_LOW,
    POWER_MODE_ULTRA_LOW
} PowerMode;

// Memory Allocation Strategies (for flexibility)
typedef enum {
    MEMORY_ALLOC_FIRST_FIT,  // Find the first available block
    MEMORY_ALLOC_BEST_FIT,    // Find the smallest suitable block
    MEMORY_ALLOC_POOL        // New pool-based allocation
} MemoryAllocationStrategy;

// Memory block types for different allocation patterns
typedef enum {
    BLOCK_TYPE_SYSTEM,    // System-critical allocations
    BLOCK_TYPE_APP,       // Application allocations
    BLOCK_TYPE_CACHE,     // Cache memory (can be reclaimed)
    BLOCK_TYPE_TEMP       // Temporary allocations
} BlockType;

// Memory pressure levels
typedef enum {
    PRESSURE_NORMAL,
    PRESSURE_MODERATE,
    PRESSURE_HIGH,
    PRESSURE_CRITICAL
} MemoryPressure;

// Memory block metadata
typedef struct {
    size_t size;
    BlockType type;
    uint32_t last_access;
    uint32_t access_count;
    bool can_relocate;
    uint8_t alignment;
} BlockMetadata;

// Memory block information (optional, for debugging/monitoring)
typedef struct {
    size_t size;        // Size of the allocated block
    void* address;      // Starting address of the allocated block
    uint32_t fragmentation_score;
    uint32_t access_count;
    uint64_t last_access_time;
} MemoryBlockInfo;

// Memory manager configuration
typedef struct {
    MemoryOptStrategy strategy;
    size_t cache_limit;           // Maximum cache memory in bytes
    size_t temp_limit;           // Maximum temporary memory in bytes
    uint32_t defrag_threshold;   // Fragmentation percentage to trigger defrag
    uint32_t cache_ttl;         // Cache time-to-live in milliseconds
    bool enable_compression;    // Enable memory compression
} MemoryConfig;

// Memory usage statistics
typedef struct {
    size_t total_memory;
    size_t used_memory;
    size_t peak_usage;
    uint32_t allocation_count;
    float fragmentation_percent;
    size_t pool_usage[3];  // Usage for tiny, small, and medium pools
} MemoryUsageStats;

// Memory statistics
typedef struct {
    size_t total_memory;
    size_t used_memory;
    size_t free_memory;
    size_t cache_memory;
    size_t temp_memory;
    uint32_t allocation_count;
    uint32_t fragmentation;
    uint32_t cache_hits;
    uint32_t cache_misses;
} MemoryStats;

// Memory optimization strategies
typedef enum {
    MEMORY_OPT_NONE,
    MEMORY_OPT_PERFORMANCE,
    MEMORY_OPT_POWER_SAVING,
    MEMORY_OPT_BALANCED
} MemoryOptStrategy;

// Function Declarations

// Initialize memory manager
//   - strategy: The allocation strategy to use
//   - power_mode: The power efficiency mode to use
void memory_init(MemoryAllocationStrategy strategy, PowerMode power_mode);

// Initialize memory manager with configuration
void memory_init_ex(const MemoryConfig* config);

// Allocate a block of memory
//   - size: The size of the memory block to allocate
// Returns:
//   - A pointer to the allocated memory block, or NULL if allocation fails
void* memory_allocate(size_t size);

// Allocate memory with specific requirements
void* memory_allocate_ex(size_t size, BlockType type, uint8_t alignment, bool can_relocate);

// Free a previously allocated block of memory
//   - ptr: A pointer to the memory block to free
void memory_free(void* ptr);

// Defragment the memory pool
void memory_defragment(void);

// Set the power efficiency mode
//   - mode: The new power efficiency mode
void memory_set_power_mode(PowerMode mode);

// Get information about a specific memory block (optional)
//   - ptr: A pointer to the memory block
// Returns:
//   - A MemoryBlockInfo structure containing information about the block, 
//     or NULL if the pointer is not valid.
MemoryBlockInfo* memory_get_block_info(void* ptr);

// Get memory usage statistics
// Returns:
//   - A MemoryUsageStats structure containing information about memory usage
MemoryUsageStats memory_get_stats(void);

// Optimize memory pools for better performance
void memory_optimize_pools(void);

// Memory optimization functions
void memory_optimize(void);
void memory_compact_cache(void);
void memory_trim_unused(void);

// Memory pressure management
MemoryPressure memory_get_pressure(void);
void memory_handle_pressure(MemoryPressure pressure);

// Cache management
void* memory_cache_alloc(size_t size);
void memory_cache_free(void* ptr);
void memory_cache_clear(void);

// Statistics and monitoring
MemoryStats memory_get_extended_stats(void);
BlockMetadata* memory_get_block_metadata(void* ptr);
void memory_print_debug_info(void);

// Power management
void memory_set_power_mode_ex(bool low_power);
void memory_optimize_for_power(void);

#endif // MEMORY_MANAGER_H
