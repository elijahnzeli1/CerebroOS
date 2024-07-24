#include "data_encryption.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAX_DATA_LENGTH 1024
#define KEY_LENGTH 32

void generate_key(unsigned char* key, size_t length) {
    srand(time(NULL));
    for (size_t i = 0; i < length; i++) {
        key[i] = rand() % 256;
    }
}

int encrypt_data(const char* data, char* encrypted_data, size_t max_length) {
    if (!data || !encrypted_data || max_length == 0) {
        return -1;  // Invalid input
    }

    size_t data_len = strlen(data);
    if (data_len >= max_length) {
        return -2;  // Input too long
    }

    unsigned char key[KEY_LENGTH];
    generate_key(key, KEY_LENGTH);

    // XOR encryption with key
    for (size_t i = 0; i < data_len; ++i) {
        encrypted_data[i] = data[i] ^ key[i % KEY_LENGTH];
    }
    encrypted_data[data_len] = '\0';

    // Append the key to the encrypted data
    if (data_len + KEY_LENGTH >= max_length) {
        return -3;  // Not enough space for key
    }
    memcpy(encrypted_data + data_len + 1, key, KEY_LENGTH);

    return 0;  // Success
}

int decrypt_data(const char* encrypted_data, char* data, size_t max_length) {
    if (!encrypted_data || !data || max_length == 0) {
        return -1;  // Invalid input
    }

    size_t encrypted_len = strlen(encrypted_data);
    if (encrypted_len + KEY_LENGTH >= max_length) {
        return -2;  // Input too long or corrupted
    }

    // Extract the key
    unsigned char key[KEY_LENGTH];
    memcpy(key, encrypted_data + encrypted_len + 1, KEY_LENGTH);

    // XOR decryption with key
    for (size_t i = 0; i < encrypted_len; ++i) {
        data[i] = encrypted_data[i] ^ key[i % KEY_LENGTH];
    }
    data[encrypted_len] = '\0';

    return 0;  // Success
}

// Example usage
int main() {
    const char* original_data = "Hello, World!";
    char encrypted[MAX_DATA_LENGTH];
    char decrypted[MAX_DATA_LENGTH];

    if (encrypt_data(original_data, encrypted, MAX_DATA_LENGTH) == 0) {
        printf("Encrypted: %s\n", encrypted);

        if (decrypt_data(encrypted, decrypted, MAX_DATA_LENGTH) == 0) {
            printf("Decrypted: %s\n", decrypted);
        } else {
            printf("Decryption failed\n");
        }
    } else {
        printf("Encryption failed\n");
    }

    return 0;
}