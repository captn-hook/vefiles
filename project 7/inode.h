#ifndef INODE_H
#define INODE_H

#include "block.h"
#include "free.h"
#include "pack.h"

#define IMAP_BLOCK 1

#define INODES_PER_BLOCK BIT * BLOCK_SIZE

#define INODE_PTR_COUNT 16

#define MAX_SYS_OPEN_FILES 64


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

struct inode *ialloc(void); // allocate a previously-free inode in the inode map

struct inode *incore_find_free(void); // This finds the first free in-core inode in the incore array. 
// It returns a pointer to that in-core inode or NULL if there are no more free in-core inodes.

struct inode *incore_find(unsigned int inode_num); // This finds an in-core inode record in the incore array by the inode number. 
// It returns a pointer to that in-core inode or NULL if it can't be found.

void incore_free_all(void); // This just sets the ref_count to all in-core inodes to 0. Useful for testing.

void read_inode(struct inode *in, int inode_num); // This takes a pointer to an empty struct inode that you're going to read the data into. The inode_num is the number of the inode you wish to read from disk.

void write_inode(struct inode *in); // This stores the inode data pointed to by in on disk. The inode_num field in the struct holds the number of the inode to be written.
// You'll have to map that inode number to a block and offset, as per above.

struct inode *iget(int inode_num); // Return a pointer to an in-core inode for the given inode number,
// or NULL on failure.

void iput(struct inode *in); // decrement the reference count on the inode. If it falls to 0, write the inode to disk.

#endif