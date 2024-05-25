#ifndef DIR_H
#define DIR_H

#include "inode.h"
#include "image.h"
#include "pack.h"

struct directory {
    struct inode *inode;
    unsigned int offset;
};

struct directory_entry {
    unsigned int inode_num;
    char name[16];
};

int directory_get(struct directory *dir, struct directory_entry *ent); //This function takes a struct directory * 
// obtained from directory_open(), and it takes a pointer to a struct directory_entry that it will fill out with the entry information.

void directory_close(struct directory *d); // iput() the directory's in-core inode to free it up. & free() the struct directory.

struct directory *directory_open(int inode_num); // We pass in an inode number and it should give us a
// struct back representing the open directory, or NULL if there was an error.

void ls(void);

int makefs(char *filename, int truncate);

void closefs(void);

#endif