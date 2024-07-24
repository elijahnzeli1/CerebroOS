#include "predictive_text.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "string_utils.h" // Assuming you have basic string functions (strlen, strncmp)

#define MAX_WORDS 100
#define MAX_WORD_LENGTH 20
#define MAX_SUGGESTIONS 5 // Maximum number of suggestions to show

// Trie Node Structure
typedef struct TrieNode {
    struct TrieNode* children[26]; // Assuming only lowercase letters
    bool isWord;
} TrieNode;

// Trie Root
static TrieNode* trie_root = NULL;

// Initialize Predictive Text
void predictive_text_init() {
    trie_root = (TrieNode*)malloc(sizeof(TrieNode));
    if (!trie_root) {
        // Handle allocation failure (e.g., display error message)
        return;
    }
    memset(trie_root->children, 0, sizeof(trie_root->children));
    trie_root->isWord = false;

    // Load Dictionary
    FILE* dictionary = fopen("dictionary.txt", "r"); // Load from a file
    if (dictionary) {
        char word[MAX_WORD_LENGTH];
        while (fscanf(dictionary, "%s", word) != EOF) {
            add_word_to_trie(word);
        }
        fclose(dictionary);
    }
}

// Add a word to the trie
void add_word_to_trie(const char* word) {
    TrieNode* node = trie_root;
    for (int i = 0; word[i] != '\0'; i++) {
        int index = word[i] - 'a';
        if (!node->children[index]) {
            node->children[index] = (TrieNode*)malloc(sizeof(TrieNode));
            memset(node->children[index]->children, 0, sizeof(node->children[index]->children));
            node->children[index]->isWord = false;
        }
        node = node->children[index];
    }
    node->isWord = true;
}

// Recursively get word suggestions from a trie node
void get_suggestions(TrieNode* node, char* prefix, char suggestions[][MAX_WORD_LENGTH], int* count) {
    if (*count >= MAX_SUGGESTIONS) {
        return; // Limit number of suggestions
    }
    if (node->isWord) {
        string_copy(suggestions[*count], prefix, MAX_WORD_LENGTH);
        (*count)++;
    }
    for (int i = 0; i < 26; i++) {
        if (node->children[i]) {
            char new_prefix[MAX_WORD_LENGTH];
            string_copy(new_prefix, prefix, MAX_WORD_LENGTH);
            new_prefix[string_length(prefix)] = 'a' + i;
            new_prefix[string_length(prefix) + 1] = '\0';
            get_suggestions(node->children[i], new_prefix, suggestions, count);
        }
    }
}

// Suggest words based on input
char** predictive_text_suggest(const char* input, int* numSuggestions) {
    TrieNode* node = trie_root;
    for (int i = 0; input[i] != '\0'; i++) {
        int index = input[i] - 'a';
        if (!node->children[index]) {
            *numSuggestions = 0;
            return NULL; // No suggestions if prefix not found
        }
        node = node->children[index];
    }

    char suggestions[MAX_SUGGESTIONS][MAX_WORD_LENGTH];
    int count = 0;
    get_suggestions(node, (char*)input, suggestions, &count);
    *numSuggestions = count;

    // Convert to array of strings (char**) for easier use (You'll need to free this later!)
    char** result = (char**)malloc(count * sizeof(char*));
    for (int i = 0; i < count; i++) {
        result[i] = (char*)malloc(MAX_WORD_LENGTH);
        strcpy(result[i], suggestions[i]);
    }
    return result;
}

// Free the trie structure
void free_trie(TrieNode* node) {
    if (node == NULL) return;
    for (int i = 0; i < 26; i++) {
        free_trie(node->children[i]);
    }
    free(node);
}

// Clean up predictive text
void predictive_text_cleanup() {
    free_trie(trie_root);
}
