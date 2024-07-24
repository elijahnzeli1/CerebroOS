#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <stddef.h>  // For size_t

// Function Prototypes with Enhanced Information:

// Calculate the length of a null-terminated string.
// 
// Parameters:
//   str: The null-terminated string whose length is to be calculated.
//
// Returns:
//   The length of the string (excluding the null terminator).
size_t string_length(const char* str);

// Copy a portion of a string.
//
// Parameters:
//   dest: The destination buffer to copy the characters into.
//   src:  The source string to copy from.
//   n:    The maximum number of characters to copy.
//
// Notes:
//   - If the source string is shorter than n, the destination is padded with null bytes.
//   - If the source string is longer than n, only n characters (including the null terminator) are copied.
void string_copy(char* dest, const char* src, size_t n);

// Optional Additional Functions:

// Concatenate two strings safely (with buffer size check).
// void string_concat(char* dest, const char* src, size_t n);

// Compare two strings (case-sensitive or case-insensitive).
// int string_compare(const char* str1, const char* str2);

// Convert a string to lowercase/uppercase.
// void string_tolower(char* str);
// void string_toupper(char* str);

// Find the first occurrence of a substring within a string.
// char* string_find(const char* str, const char* substr);

// Tokenize a string based on delimiters.
// char** string_tokenize(const char* str, const char* delim);

#endif // STRING_UTILS_H
