#include <string.h>
#include <stdio.h>
#include "dir.h"

int makefs(char *filename, int truncate) {
    //open image file
    if (image_open(filename, truncate) < 0) {
        return -1;
    }

    struct inode *root_inode = ialloc();
    int inode_num = root_inode->inode_num;
    int block_num = alloc();

    root_inode->flags = 2;
    root_inode->size = 64;
    root_inode->block_ptr[0] = block_num;

    unsigned char block[BLOCK_SIZE];

    write_u16(block, inode_num);
    strcpy((char *) (block + 2), "."); //current directory
    write_u16(block + 32, inode_num);
    strcpy((char *) (block + 34), ".."); //parent directory

    bwrite(block_num, block);

    iput(root_inode);

    return 0;
}

void closefs(void) {
    image_close();
}

void ls(void) {
    struct directory *dir;
    struct directory_entry ent;

    dir = directory_open(0);

    while (directory_get(dir, &ent) != -1) {
        printf("%d %s\n", ent.inode_num, ent.name);
    }

    directory_close(dir);
}

int directory_get(struct directory *dir, struct directory_entry *ent) {
    unsigned char block[BLOCK_SIZE];
    struct inode *inode = dir->inode;

    if (dir->offset >= inode->size) {
        return -1; //end of directory
    }

    bread(inode->block_ptr[0], block); 
    memcpy(ent, block + dir->offset, sizeof(struct directory_entry)); // move the data from the block into the struct directory_entry
    dir->offset += sizeof(struct directory_entry); // move the offset to the next entry

    return 0;
}

void directory_close(struct directory *d) {
    iput(d->inode);
    free(d);
}

struct directory *directory_open(int inode_num) {
    struct directory *dir = malloc(sizeof(struct directory)); // allocate memory for the struct directory
    dir->inode = iget(inode_num); // get the inode for the directory
    dir->offset = 0; // set the offset to 0
    
    return dir;
}