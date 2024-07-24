#ifndef DATA_ENCRYPTION_H
#define DATA_ENCRYPTION_H

#include <stddef.h>

#define MAX_DATA_LENGTH 1024
#define KEY_LENGTH 32

/**
 * @brief Generate a random encryption key
 * 
 * @param key Pointer to the buffer where the key will be stored
 * @param length Length of the key to generate
 */
void generate_key(unsigned char* key, size_t length);

/**
 * @brief Encrypt the given data
 * 
 * @param data The data to encrypt
 * @param encrypted_data Buffer to store the encrypted data
 * @param max_length Maximum length of the encrypted_data buffer
 * @return int 0 on success, negative value on error
 */
int encrypt_data(const char* data, char* encrypted_data, size_t max_length);

/**
 * @brief Decrypt the given data
 * 
 * @param encrypted_data The data to decrypt
 * @param data Buffer to store the decrypted data
 * @param max_length Maximum length of the data buffer
 * @return int 0 on success, negative value on error
 */
int decrypt_data(const char* encrypted_data, char* data, size_t max_length);

#endif // DATA_ENCRYPTION_H