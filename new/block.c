#include "block.h"

// read
unsigned char *bread(int block_num, unsigned char *block) {
    // seek to the block_num
    if (lseek(image_fd, block_num * BLOCK_SIZE, SEEK_SET) < 0) {
        perror("bread");
        return NULL;
    }

    if (read(image_fd, block, BLOCK_SIZE) < 0) {
        perror("bread");
        return NULL;
    }

    return block;
}

// write
void bwrite(int block_num, unsigned char *block) {
    // seek to the block_num
    if (lseek(image_fd, block_num * BLOCK_SIZE, SEEK_SET) < 0) {
        perror("bwrite");
        return;
    }

    if (write(image_fd, block, BLOCK_SIZE) < 0) {
        perror("bwrite");
        return;
    }
}