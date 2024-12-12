#include <iostream>
#include <cstdint>

#include "magma.h"
#include "magma.c"

class Number
{
public:
    Number():
        value(0)
    {}
    Number(uint64_t value_):
        value(value_)
    {}

    operator uint64_t() const
    { return value; }

public:
    friend Number operator+(Number left, Number right);
    friend Number operator+(Number left, uint64_t right);
    friend Number operator+(uint64_t left, Number right);
    friend Number operator*(Number left, Number right);

public:
    friend std::ostream& operator<<(std::ostream& os, Number number)
    { return os << number.value; }

public:
    uint64_t value;
};

Number operator+(Number left, Number right)
{ return left.value ^ right.value; }

Number operator+(Number left, uint64_t right)
{
    return Number(left.value ^ right);
}

Number operator+(uint64_t left, Number right)
{
    return Number(left ^ right.value);
}

uint64_t MulMod2(uint64_t left, uint64_t right)
{
    uint64_t result = 0;
    uint64_t offset = 0;
    while (right)
    {
        if (right & 1)
            result ^= left << offset;
        ++offset;
        right >>= 1;
    }
    return result;
}

size_t maxbit(uint64_t value)
{
    size_t retval = 0;
    while (value)
    {
        value >>= 1;
        ++retval;
    }
    return retval;
}

uint64_t ModGenP(uint64_t value, uint64_t gen_polynom)
{
    while (true)
    {
        size_t vmaxbit = maxbit(value);
        size_t pmaxbit = maxbit(gen_polynom);

        if (vmaxbit < pmaxbit)
            return value;

        size_t quotient = 1ULL << (vmaxbit - pmaxbit);
        value ^= gen_polynom * quotient;
    }
}

Number operator*(Number left, Number right)
{
    uint64_t gen_p = (1ULL << 63) + (1ULL << 46) + (1ULL << 10) + (1ULL << 5) + 1;
    uint64_t value = MulMod2(left.value, right.value);
    return ModGenP(value, gen_p);
}

uint64_t MSB(uint64_t number, size_t size)
{ return number & (0xffffffffffffffffULL - ((1ULL << size) - 1)); }

constexpr uint64_t Get0orN()
{
    constexpr uint64_t nonce = 0x5678;
    constexpr uint64_t zero  = 0;

    return (zero << 63) + nonce;
}

constexpr uint64_t Get1orN()
{
    constexpr uint64_t nonce = 0x5678;
    constexpr uint64_t one   = 1;

    return (one << 63) + nonce;
}

uint64_t Shifr(uint64_t input) {
    uint8_t plaintext[BLOCK_SIZE] = {0};
    uint8_t ciphertext[BLOCK_SIZE] = {0};

    ((uint64_t *)plaintext)[0] = input;

    magma_encrypt(key, plaintext, ciphertext);

    return ((uint64_t *)ciphertext)[0];
}

void FillY(Number* Y, size_t count)
{
    Y[0] = Shifr(Get0orN());
    for (size_t i = 1; i < count; ++i)
        Y[i] = Y[i - 1] + 1;
}

void FillZ(Number* Z, size_t count)
{
    Z[0] = Shifr(Get1orN());
    for (size_t i = 1; i < count; ++i)
        Z[i] = Z[i - 1] + (1ULL << 32);
}

void FillH(Number* H, Number* Z, size_t count)
{
    for (size_t i = 0; i < count; ++i)
        H[i] = Shifr(Z[i]);
}

void DecryptText(Number* C, Number* P, Number* Y, size_t count)
{
    for (size_t i = 0; i < count; ++i)
    {
        uint64_t shifted_Y = Shifr(Y[i].value); 
        P[i] = Number(C[i].value ^ shifted_Y);  
    }
}


int main()
{
    constexpr size_t N = 11;
    constexpr size_t M = 11;

    Number Y[N];
    Number Z[N + M + 1];

    FillY(Y, N);
    FillZ(Z, N + M + 1);

    Number P[N] = {0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0x10, 0x11};
    Number C[N];

    for (size_t i = 0; i < N; ++i) C[i] = Number(Shifr(Y[i])) + P[i];

    Number P_decrypted[N];
    DecryptText(C, P_decrypted, Y, N);

    std::cout << "Encrypted text (C): ";
    for (size_t i = 0; i < N; ++i)
    {
        std::cout << std::hex << C[i];
        std::cout << ", ";
    }
    std::cout << std::endl;

    std::cout << "Decrypted text (P): ";
    for (size_t i = 0; i < N; ++i)
    {
        std::cout << "0x" << std::hex << P_decrypted[i];
        std::cout << ", ";
    }
    std::cout << std::endl;

    return 0;
}

