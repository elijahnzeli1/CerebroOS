#ifndef PREDICTIVE_TEXT_H
#define PREDICTIVE_TEXT_H

#include <stddef.h>  // for size_t

// Error Codes for Predictive Text
typedef enum {
    PREDICTIVE_TEXT_ERROR_NONE = 0,           // No error
    PREDICTIVE_TEXT_ERROR_DICTIONARY_LOAD,   // Failed to load dictionary
    PREDICTIVE_TEXT_ERROR_MEMORY,            // Memory allocation error
    // ... add more error codes if needed
} PredictiveTextError;

// Function Declarations

// Initialize predictive text engine
// - dictionaryPath: Path to the dictionary file (optional, if using internal dictionary)
PredictiveTextError predictive_text_init(const char* dictionaryPath);

// Get word suggestions based on input text
// - input: The input text (prefix)
// - numSuggestions: Pointer to an integer where the number of suggestions will be stored
// Returns:
//   - An array of strings containing the suggestions (caller must free this memory)
//   - NULL if no suggestions are found or an error occurred.
char** predictive_text_suggest(const char* input, int* numSuggestions);

// Free memory allocated by the predictive text engine
void predictive_text_cleanup();

// Optional Functions:

// Add a word to the dictionary (for user-defined words)
PredictiveTextError predictive_text_add_word(const char* word);

// Remove a word from the dictionary
PredictiveTextError predictive_text_remove_word(const char* word);

// Get the current language setting
// const char* predictive_text_get_language();

// Set the current language setting
// PredictiveTextError predictive_text_set_language(const char* language);

#endif // PREDICTIVE_TEXT_H
