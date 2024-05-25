#include "inode.h"

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