#include "inode.h"

static struct inode incore[MAX_SYS_OPEN_FILES] = {0};

int ialloc(void) {
    // read the inode map
    unsigned char imap[BLOCK_SIZE];
    if (bread(IMAP_BLOCK, imap) == NULL) {
        return -1;
    }

    int free_inode = find_free(imap);
    if (free_inode == -1) {
        return -1;
    } else {
        set_free(imap, free_inode, 1);
        if (bwrite(IMAP_BLOCK, imap) < 0) {
            return -1;
        }
        return free_inode;
    }
}

struct inode *incore_find_free(void) {
    for (int i = 0; i < MAX_SYS_OPEN_FILES; i++) {
        if (incore[i].ref_count == 0) {
            return &incore[i]; // dereference the pointer to the incore inode
        }
    }
    return NULL;
}

struct inode *incore_find(unsigned int inode_num) {
    for (int i = 0; i < MAX_SYS_OPEN_FILES; i++) {
        if (incore[i].ref_count != 0 && incore[i].inode_num == inode_num) {
            return &incore[i];
        }
    }
    return NULL;
}

void incore_free_all(void) {
    for (int i = 0; i < MAX_SYS_OPEN_FILES; i++) {
        incore[i].ref_count = 0;
    }
}