#include <string.h>
#include <stdio.h>
#include "dir.h"

int makefs(char *filename, int truncate) {
    //open image file
    if (image_open(filename, truncate) < 0) {
        return -1;
    }

    struct inode *root_inode = ialloc();
    int block_num = alloc();

    root_inode->inode_num = 0;
    root_inode->flags = 2;
    root_inode->size = DIR_SIZE * 2;
    root_inode->block_ptr[0] = block_num;

    unsigned char block[BLOCK_SIZE] = {0};

    // add . and .. entries to the root directory
    write_u16(block, root_inode->inode_num);
    strcpy((char *) (block + INODE_ENT), ".\0");
    write_u16(block + DIR_SIZE, root_inode->inode_num);
    strcpy((char *) (block + INODE_ENT + DIR_SIZE), "..\0");

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
    if (dir->offset + DIR_SIZE > dir->inode->size) {
        return -1;
    }

    int data_block_index = dir->offset / BLOCK_SIZE;

    int data_block_num = dir->inode->block_ptr[data_block_index];

    unsigned char block[BLOCK_SIZE] = {0};
    bread(data_block_num, block);

    int offset_in_block = dir->offset % BLOCK_SIZE;

    ent->inode_num = read_u16(block + offset_in_block);

    // idk man this is hacky
    char name[NAME_MAX + 1];
    strncpy(name, (char *) (block + offset_in_block + INODE_ENT), NAME_MAX);
    name[NAME_MAX] = '\0';
    strcpy(ent->name, name);
    
    dir->offset += DIR_SIZE;

    return 0;
}
void directory_close(struct directory *d) {
    iput(d->inode);
    free(d);
}

struct directory *directory_open(int inode_num) {
    struct inode *inode = iget(inode_num);
    if (inode == NULL) {
        return NULL;
    }

    struct directory *dir = malloc(sizeof(struct directory));

    dir->inode = inode;
    dir->offset = 0;

    return dir;
}