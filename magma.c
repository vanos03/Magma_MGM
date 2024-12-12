#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define BLOCK_SIZE 8
#define ROUNDS 32

// Пример S-блоков (замените на реальные S-блоки для настоящей реализации)
static const uint8_t S_BOX[8][16] = {
    {12, 4, 6, 2, 10, 5, 11, 9, 14, 8, 13, 7, 0, 3, 15, 1},
    {6, 8, 2, 3, 9, 10, 5, 12, 1, 14, 4, 7, 0, 15, 11, 13},
    {11, 3, 5, 8, 2, 15, 10, 13, 14, 1, 7, 4, 12, 9, 6, 0},
    {12, 8, 2, 1, 13, 4, 10, 6, 15, 11, 5, 14, 9, 0, 3, 7},
    {7, 15, 5, 10, 8, 1, 6, 13, 0, 9, 3, 14, 11, 4, 2, 12},
    {5, 13, 15, 6, 9, 2, 12, 10, 11, 7, 8, 1, 4, 3, 14, 0},
    {8, 14, 2, 5, 6, 9, 1, 12, 15, 4, 11, 0, 13, 10, 3, 7},
    {1, 7, 14, 13, 0, 5, 8, 3, 4, 15, 10, 6, 9, 12, 11, 2}
};

// Функция замены через S-блоки
uint32_t substitute(uint32_t value) {
    uint32_t result = 0;
    for (int i = 0; i < 8; ++i) {
        uint8_t nibble = (value >> (4 * i)) & 0xF;
        result |= S_BOX[i][nibble] << (4 * i);
    }
    return result;
}

// Функция шифрования одного раунда
uint32_t round_encrypt(uint32_t block, uint32_t key) {
    uint32_t temp = (block + key) % 0x100000000;
    temp = substitute(temp);
    return (temp << 11) | (temp >> (32 - 11)); // Циклический сдвиг влево на 11 бит
}

// Основная функция шифрования
void magma_encrypt(const uint32_t *key, const uint8_t *input, uint8_t *output) {
    uint32_t left = ((uint32_t *)input)[0];
    uint32_t right = ((uint32_t *)input)[1];

    for (int i = 0; i < ROUNDS; ++i) {
        uint32_t temp = right;
        right = left ^ round_encrypt(right, key[i % 8]);
        left = temp;
    }

    ((uint32_t *)output)[0] = right;
    ((uint32_t *)output)[1] = left;
}

// Основная функция расшифрования
void magma_decrypt(const uint32_t *key, const uint8_t *input, uint8_t *output) {
    uint32_t left = ((uint32_t *)input)[0];
    uint32_t right = ((uint32_t *)input)[1];

    for (int i = 0; i < ROUNDS; ++i) {
        uint32_t temp = right;
        right = left ^ round_encrypt(right, key[(31 - i) % 8]);
        left = temp;
    }

    ((uint32_t *)output)[0] = right;
    ((uint32_t *)output)[1] = left;
}

// int main() {
//     // Пример ключа (256 бит)
//     uint32_t key[8] = {0x01234567, 0x89ABCDEF, 0xFEDCBA98, 0x76543210,
//                        0x0F1E2D3C, 0x4B5A6978, 0x89ABCDEF, 0x01234567};

//     // Входные данные (8 байт)
//     uint8_t plaintext[BLOCK_SIZE] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
//     uint8_t ciphertext[BLOCK_SIZE];
//     uint8_t decrypted[BLOCK_SIZE];

//     // Шифрование
//     magma_encrypt(key, plaintext, ciphertext);
//     printf("Ciphertext: ");
//     for (int i = 0; i < BLOCK_SIZE; ++i) {
//         printf("%02X ", ciphertext[i]);
//     }
//     printf("\n");

//     // Расшифрование
//     magma_decrypt(key, ciphertext, decrypted);
//     printf("Decrypted: ");
//     for (int i = 0; i < BLOCK_SIZE; ++i) {
//         printf("%02X ", decrypted[i]);
//     }
//     printf("\n");

//     return 0;
// }
