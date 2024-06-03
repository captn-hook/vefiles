#include "inode.h"

static struct inode incore[MAX_SYS_OPEN_FILES] = {0};

struct inode *ialloc(void) {
    // read the inode map
    unsigned char imap[BLOCK_SIZE];
    if (bread(IMAP_BLOCK, imap) == NULL) {
        return NULL;
    }

    unsigned int free_inode_num = find_free(imap);

    struct inode *in = iget(free_inode_num);
    if (in == NULL) {
        return NULL;
    }
    
    
    in->inode_num = free_inode_num;
    in->ref_count = 1;
    in->size = 0;
    in->owner_id = 0;
    in->permissions = 0;
    in->flags = 0;

    
    set_free(imap, in->inode_num, 1);
    
    for (int i = 0; i < INODE_PTR_COUNT; i++) {
        in->block_ptr[i] = 0;
    }

    write_inode(in);
    bwrite(IMAP_BLOCK, imap);

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
    int block_num = inode_num / INODES_PER_BLOCK + 1;
    int offset = (inode_num % INODES_PER_BLOCK) * INODE_SIZE;

    unsigned char block[BLOCK_SIZE];
    if (bread(block_num, block) == NULL) {
        return;
    }

    in->size = read_u32(&block[offset]);
    in->owner_id = read_u16(&block[offset + 4]);
    in->permissions = read_u8(&block[offset + 6]);
    in->flags = read_u8(&block[offset + 7]);
    in->link_count = read_u8(&block[offset + 8]);
    for (int i = 0; i < INODE_PTR_COUNT; i++) {
        in->block_ptr[i] = read_u16(&block[offset + 9 + i*2]);
    }

    in->inode_num = inode_num;
    in->ref_count = 1;    
}

void write_inode(struct inode *in) {
    int block_num = in->inode_num / INODES_PER_BLOCK + 1;
    int offset = (in->inode_num % INODES_PER_BLOCK) * INODE_SIZE;

    unsigned char block[BLOCK_SIZE];
    if (bread(block_num, block) == NULL) {
        printf("bread failed\n");
        return;
    }

    write_u32(&block[offset], in->size);
    write_u16(&block[offset + 4], in->owner_id);
    write_u8(&block[offset + 6], in->permissions);
    write_u8(&block[offset + 7], in->flags);
    write_u8(&block[offset + 8], in->link_count);
    for (int i = 0; i < INODE_PTR_COUNT; i++) {
        write_u16(&block[offset + 9 + i*2], in->block_ptr[i]);
    }

    if (bwrite(block_num, block) < 0) {
        printf("bwrite failed\n");
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

void print_inodes(void) {
    printf("inodes:\n");
    for (int i = 0; i < MAX_SYS_OPEN_FILES; i++) {
        if (incore[i].ref_count != 0) {
            printf("inode %d: size %d, owner %d, permissions %d, flags %d, link count %d, ref count %d\n", incore[i].inode_num, incore[i].size, incore[i].owner_id, incore[i].permissions, incore[i].flags, incore[i].link_count, incore[i].ref_count);
        }
    }
    printf("------------    \n");
}