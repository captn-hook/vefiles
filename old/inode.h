#ifndef INODE_H
#define INODE_H

#include "block.h"
#include "free.h"

#define IMAP_BLOCK 1

#define INODES_PER_BLOCK BIT * BLOCK_SIZE

int ialloc(void); // allocate a previously-free inode in the inode map

#endif