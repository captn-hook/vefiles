#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include "image.h"
#include "block.h"

// read
unsigned char *bread(int block_num, unsigned char *block) {
    // Seek to the block_num
    if (lseek(image_fd, block_num * BLOCK_SIZE, SEEK_SET) < 0) {
        perror("bread");
        return NULL;
    }

    // Read the block
    if (read(image_fd, block, BLOCK_SIZE) < 0) {
        perror("bread");
        return NULL;
    }

    return block;
}

// write
void bwrite(int block_num, unsigned char *block) {
    // Seek to the block_num
    if (lseek(image_fd, block_num * BLOCK_SIZE, SEEK_SET) < 0) {
        perror("bwrite");
        return;
    }

    // Write the block
    if (write(image_fd, block, BLOCK_SIZE) < 0) {
        perror("bwrite");
        return;
    }
}