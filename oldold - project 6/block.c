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
int bwrite(int block_num, unsigned char *block) {
    // seek to the block_num
    if (lseek(image_fd, block_num * BLOCK_SIZE, SEEK_SET) < 0) {
        perror("bwrite");
        return -1;
    }

    if (write(image_fd, block, BLOCK_SIZE) < 0) {
        perror("bwrite");
        return -1;
    }

    return 0;
}

int alloc(void) {
    // read the block map
    unsigned char bmap[BLOCK_SIZE];
    if (bread(BMAP_BLOCK, bmap) == NULL) {
        return -1;
    }
    
    int free_block = find_free(bmap);
    if (free_block == -1) {
        return -1;
    } else {
        set_free(bmap, free_block, 1);
        if (bwrite(BMAP_BLOCK, bmap) < 0) {
            return -1;
        }
        return free_block;
    }
}