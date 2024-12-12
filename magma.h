#ifndef MAGMA_H
#define MAGMA_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define BLOCK_SIZE 8
#define ROUNDS 32

uint32_t key[8] = {0x01234567, 0x89ABCDEF, 0xFEDCBA98, 0x76543210,
                       0x0F1E2D3C, 0x4B5A6978, 0x89ABCDEF, 0x01234567};

void magma_encrypt(const uint32_t *key, const uint8_t *input, uint8_t *output);
void magma_decrypt(const uint32_t *key, const uint8_t *input, uint8_t *output);

#endif 
