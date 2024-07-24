#include "string_utils.h"
#include <stddef.h>  // For size_t

// Optimized String Length Calculation
size_t string_length(const char* str) {
    const char* end = str;  // Pointer to the end of the string
    while (*end) {           // Loop until we reach the null terminator '\0'
        end++;               // Increment the end pointer
    }
    return end - str;        // Calculate the length by subtracting the start address
}

// Safe and Efficient String Copy (with null terminator guarantee)
void string_copy(char* dest, const char* src, size_t n) {
    if (dest == NULL || src == NULL) {
        return; // Handle null pointers gracefully
    }

    size_t i = 0;
    while (i < n && src[i] != '\0') { // Copy characters until n-1 or '\0' is reached
        dest[i] = src[i];
        i++;
    }

    // Ensure null termination even if source string is longer than n
    if (i == n) {
        dest[n - 1] = '\0';  // Overwrite last character with '\0'
    } else {
        dest[i] = '\0';       // Null terminate at the current position
    }
}

// Additional String Utilities (for potential future use)

// String Concatenation (with buffer size check)
// void string_concat(char* dest, const char* src, size_t n) { /* ... */ }

// String Comparison (case-sensitive)
// int string_compare(const char* str1, const char* str2) { /* ... */ }

// String Search (find first occurrence of substring)
// char* string_search(const char* str, const char* substr) { /* ... */ }

// ... (other functions like tolower, toupper, etc.)
