#include "free.h"

int find_low_clear_bit(unsigned char x)
{
    for (int i = 0; i < 8; i++)
        if (!(x & (1 << i)))
            return i;
    
    return -1;
}

void set_free(unsigned char *block, int num, int set) {
    int byte_num = num / BIT;
    int bit_num = num % BIT;
    unsigned char mask = 1 << bit_num;
    if (set) {
        block[byte_num] |= mask;
    } else {
        block[byte_num] &= ~mask;
    }
}

int find_free(unsigned char *block) {
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        if (block[i] != 0xff)
            return i * BIT + find_low_clear_bit(block[i]);
    }
    return -1;
}