#include "inode.h"

static struct inode incore[MAX_SYS_OPEN_FILES] = {0};

struct inode *ialloc(void) {
    // read the inode map
    unsigned char imap[BLOCK_SIZE];
    if (bread(IMAP_BLOCK, imap) == NULL) {
        return NULL;
    }

    struct inode *in = incore_find_free();
    if (in == NULL) {
        return NULL;
    }

    set_free(imap, in->inode_num, 0);
    if (bwrite(IMAP_BLOCK, imap) < 0) {
        return NULL;
    }

    // the inode map
    in->size = 0;
    in->owner_id = 0;
    in->permissions = 0;
    in->flags = 0;
    in->link_count = 0;
    for (int i = 0; i < INODE_PTR_COUNT; i++) {
        in->block_ptr[i] = 0;
    }
    in->inode_num = find_free(imap);

    write_inode(in);

    return in; // ref
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

void read_inode(struct inode *in, int inode_num) {
    unsigned char block[BLOCK_SIZE];
    if (bread(inode_num, block) == NULL) {
        return;
    }
    unpack_inode(block, in);
    in->inode_num = inode_num;
}

void write_inode(struct inode *in) {
    unsigned char block[BLOCK_SIZE];
    pack_inode(block, in);
    if (bwrite(in->inode_num, block) < 0) {
        return;
    }
}

struct inode *iget(int inode_num) {
    struct inode *in = incore_find(inode_num);
    if (in != NULL) {
        in->ref_count++;
        return in;
    }

    in = incore_find_free();
    if (in == NULL) {
        return NULL;
    }

    read_inode(in, inode_num);
    in->ref_count = 1;
    in->inode_num = inode_num; // set in-core data

    return in;
}

void iput(struct inode *in) {
    if (in->ref_count == 0) {
        return;
    }

    in->ref_count--;

    if (in->ref_count == 0) {
        write_inode(in);
    }
}