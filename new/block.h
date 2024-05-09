#ifndef BLOCK_H
#define BLOCK_H

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include "image.h"

#define BLOCK_SIZE 4096

unsigned char *bread(int block_num, unsigned char *block);
void bwrite(int block_num, unsigned char *block);

int alloc(void); // allocate a previously-free data block from the block map.

#endif