#include <string.h>
#include <stdio.h>
#include "dir.h"

int makefs(char *filename, int truncate)
{
    // open image file
    if (image_open(filename, truncate) < 0)
    {
        return -1;
    }

    struct inode *root_inode = ialloc();

    int block_num = alloc();

    root_inode->inode_num = 0;
    root_inode->flags = 2;
    root_inode->size = DIR_SIZE * 2;
    root_inode->block_ptr[0] = block_num;
    root_inode->ref_count = 1; // ref count doesnt always work

    unsigned char block[BLOCK_SIZE] = {0};

    // add . and .. entries to the root directory
    write_u16(block, root_inode->inode_num);
    strcpy((char *)(block + INODE_ENT), ".\0");
    write_u16(block + DIR_SIZE, root_inode->inode_num);
    strcpy((char *)(block + INODE_ENT + DIR_SIZE), "..\0");

    bwrite(block_num, block);
    iput(root_inode);

    return 0;
}

void closefs(void)
{
    image_close();
}

void ls(void)
{
    printf("ls:\n");
    struct directory *dir;
    struct directory_entry ent;

    dir = directory_open(0);

    while (directory_get(dir, &ent) != -1)
    {
        printf("%d %s\n", ent.inode_num, ent.name);
    }

    directory_close(dir);
    printf("------------    \n");
}

int directory_get(struct directory *dir, struct directory_entry *ent)
{
    if (dir->offset + DIR_SIZE > dir->inode->size)
    {
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
    strncpy(name, (char *)(block + offset_in_block + INODE_ENT), NAME_MAX);
    name[NAME_MAX] = '\0';
    strcpy(ent->name, name);

    dir->offset += DIR_SIZE;

    return 0;
}

void directory_close(struct directory *d)
{
    iput(d->inode);
    free(d);
}

struct directory *directory_open(int inode_num)
{
    struct inode *inode = iget(inode_num);
    if (inode == NULL)
    {
        return NULL;
    }

    struct directory *dir = malloc(sizeof(struct directory));

    dir->inode = inode;
    dir->offset = 0;

    return dir;
}

// Inspired by the `dirname` and `basename` Unix utilities
//
// But doesn't handle a trailing '/' on the path correctly.
//
// So don't do that.
char *get_dirname(const char *path, char *dirname)
{
    strcpy(dirname, path);
    char *p = strrchr(dirname, '/');
    if (p == NULL)
    {
        strcpy(dirname, ".");
        return dirname;
    }
    if (p == dirname) // Last slash is the root /
        *(p + 1) = '\0';
    else
        *p = '\0'; // Last slash is not the root /
    return dirname;
}
char *get_basename(const char *path, char *basename)
{
    if (strcmp(path, "/") == 0)
    {
        strcpy(basename, path);
        return basename;
    }
    const char *p = strrchr(path, '/');
    if (p == NULL)
        p = path; // No slash in name, start at beginning
    else
        p++; // Start just after slash
    strcpy(basename, p);
    return basename;
}

struct inode *namei(char *path)
{
    // If the path is /, it returns the root directory's in-core inode.
    // If the path is /foo, it returns foo's in-core inode.
    // If the path is /foo/bar, it returns bar's in-core inode.
    // If the path is invalid (i.e. a component isn't found), it returns NULL.
    
    char dirname[NAME_MAX + 1];
    char basename[NAME_MAX + 1];
    struct inode *dir_inode;
    struct directory *dir;
    struct directory_entry ent;
    struct inode *in;

    if (strcmp(path, "/") == 0)
    {
        return iget(ROOT_INODE_NUM);
    }

    get_dirname(path, dirname);
    get_basename(path, basename);

    dir_inode = namei(dirname);
    if (dir_inode == NULL)
    {
        return NULL;
    } 

    dir = directory_open(dir_inode->inode_num);
    while (directory_get(dir, &ent) != -1)
    {
        if (strcmp(ent.name, basename) == 0)
        {
            in = iget(ent.inode_num);
            directory_close(dir);
            return in;
        }
    }

    directory_close(dir);
    return NULL;
}

int directory_make(char *path) {

    if (namei(path) != NULL)
    {
        printf("namei(path) != NULL\n");
        return -1;
    }

    // Find the directory path that will contain the new directory. This should be every component of the path except the last one. In this case, the parent directory path will be / because that's where what going to create the new directory within.    
    // Find the new directory name from the path. To do these first two steps, here are some helper functions Download here are some helper functions.
    char parent_path[NAME_MAX];
    char new_dir_name[NAME_MAX];
    get_dirname(path, parent_path);
    get_basename(path, new_dir_name);

    // Find the inode for the parent directory that will hold the new entry (namei()).

    struct inode *parent_inode = namei(parent_path);
    if (parent_inode == NULL)
    {
        printf("parent_inode is null\n");
        return -1;
    }

    // Create a new inode for the new directory (ialloc()).

    struct inode *new_inode = ialloc();
    if (new_inode == NULL)
    {
        printf("new_inode is null\n");
        return -1;
    }

    // Create a new data block for the new directory entries (alloc()).

    int block_num = alloc();
    if (block_num == -1)
    {
        printf("block_num is -1\n");
        return -1;
    }

    // Create a new block-sized array for the new directory data block and initialize it . and .. files. The . should contain the new directory's inode number. The .. should contain the parent directory's inode number. This is like we did for the root directory in the previous project.

    unsigned char block[BLOCK_SIZE] = {0};

    write_u16(block, new_inode->inode_num);
    strcpy((char *)(block + INODE_ENT), ".\0");
    write_u16(block + DIR_SIZE, parent_inode->inode_num);
    strcpy((char *)(block + INODE_ENT + DIR_SIZE), "..\0");

    // Initialize the new directory in-core inode with a proper size and other fields, similar to how we did with the hard-coded root directory in the previous project.

    new_inode->inode_num = new_inode->inode_num;
    new_inode->flags = 2;
    new_inode->size = DIR_SIZE * 2;
    new_inode->block_ptr[0] = block_num;
    new_inode->ref_count = 1;

    // Write the new directory data block to disk (bwrite()).

    bwrite(block_num, block);

    // From the parent directory inode, find the block that will contain the new directory entry (using the size and block_ptr fields).

    int parent_block_num = parent_inode->block_ptr[0];
    int parent_block_offset = parent_inode->size;
    
    // Read that block into memory unless you're creating a new one (bread()), and add the new directory entry to it.

    if (parent_block_offset % BLOCK_SIZE == 0)
    {
        parent_block_num = alloc();
        if (parent_block_num == -1)
        {
            printf("parent_block_num is -1\n");
            return -1;
        }
    } else {
        bread(parent_block_num, block);
    }

    // Write that block to disk (bwrite()).

    write_u16(block + parent_block_offset, new_inode->inode_num);
    strcpy((char *)(block + parent_block_offset + INODE_ENT), new_dir_name);
    bwrite(parent_block_num, block);

    // Update the parent directory's size field (which should increase by 32, the size of the new directory entry.

    parent_inode->size += DIR_SIZE;

    // Release the new directory's in-core inode (iput()).
    iput(new_inode);
    // Release the parent directory's in-core inode (iput()).
    iput(parent_inode);

    return 0;
}