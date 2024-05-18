#ifndef INODE_H
#define INODE_H

#include "block.h"
#include "free.h"

#define IMAP_BLOCK 1

#define INODES_PER_BLOCK BIT * BLOCK_SIZE

#define INODE_PTR_COUNT 16

#define MAX_SYS_OPEN_FILES 64

int ialloc(void); // allocate a previously-free inode in the inode map

struct inode {
    unsigned int size;
    unsigned short owner_id;
    unsigned char permissions;
    unsigned char flags;
    unsigned char link_count;
    unsigned short block_ptr[INODE_PTR_COUNT];

    unsigned int ref_count;  // in-core only
    unsigned int inode_num;  // in-core only
};

struct inode *incore_find_free(void); // This finds the first free in-core inode in the incore array. 
// It returns a pointer to that in-core inode or NULL if there are no more free in-core inodes.

struct inode *incore_find(unsigned int inode_num); // This finds an in-core inode record in the incore array by the inode number. 
// It returns a pointer to that in-core inode or NULL if it can't be found.

void incore_free_all(void); // This just sets the ref_count to all in-core inodes to 0. Useful for testing.

#endif