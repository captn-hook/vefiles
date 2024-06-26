#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "ctest.h"

#include "image.h"
#include "block.h"
#include "free.h"
#include "inode.h"
#include "dir.h"

#ifdef CTEST_ENABLE

void test1()
{
    struct inode *inode = ialloc();
    CTEST_ASSERT(inode != NULL, "inode != NULL");
    CTEST_ASSERT(inode->inode_num == 0, "inode->inode_num == 0");

    int block_num = alloc();
    CTEST_ASSERT(block_num == 1, "block_num == 1");
}

int test2()
{
    // write a large amount of data
    unsigned char block[4096];
    for (int i = 0; i < 4096; i++)
    {
        block[i] = i % 256;
    }

    for (int i = 0; i < 1000; i++)
    {
        bwrite(i, block);
    }

    // read the data back
    int return_value = 0;
    for (int i = 0; i < 1000; i++)
    {
        bread(i, block);
        for (int j = 0; j < 4096; j++)
        {
            CTEST_ASSERT(block[j] == j % 256, "block[j] == j % 256");
            if (block[j] != j % 256)
            {
                return_value = -1;
            }
        }
    }

    return return_value;
}

void test2andahalf()
{
    // test read inode and write inode
    struct inode *inode = incore_find_free();
    CTEST_ASSERT(inode != NULL, "inode != NULL");
    inode->size = 100;
    inode->owner_id = 1;
    inode->permissions = 2;
    inode->flags = 3;
    inode->link_count = 4;

    for (int i = 0; i < INODE_PTR_COUNT; i++)
    {
        inode->block_ptr[i] = i;
    }

    write_inode(inode);
    struct inode *inode2 = incore_find(inode->inode_num);
    CTEST_ASSERT(inode2 != NULL, "inode2 != NULL");
}

void test3()
{

    struct inode *inode = incore_find_free();
    CTEST_ASSERT(inode != NULL, "inode != NULL");

    inode->ref_count = 1;
    inode->flags = 1;

    incore_free_all();
    CTEST_ASSERT(inode->ref_count == 0, "inode->ref_count == 0");
}

void test4()
{
    // test iget and iput
    struct inode *inode = iget(0);
    CTEST_ASSERT(inode != NULL, "inode != NULL");

    struct inode *inode2 = iget(0);
    CTEST_ASSERT(inode2 != NULL, "inode2 != NULL");
    CTEST_ASSERT(inode->ref_count == 2, "inode->ref_count == 2");

    iput(inode);
    CTEST_ASSERT(inode->ref_count == 1, "inode->ref_count == 1");
}

void test5()
{
    // test dir
    struct directory *dir = directory_open(0);

    struct directory_entry ent;
    while (directory_get(dir, &ent) != -1)
    {
        printf("%d %s\n", ent.inode_num, ent.name);
    }

    CTEST_ASSERT(directory_get(dir, &ent) == -1, "directory_get(dir, &ent) == -1");

    directory_close(dir);
}

void test6()
{
    // test dir more
    struct inode *root_inode = iget(0);
    struct directory *dir = directory_open(root_inode->inode_num);

    struct directory_entry ent;

    CTEST_ASSERT(directory_get(dir, &ent) == 0, "directory_get(dir, &ent) == 0");

    directory_close(dir);
}

void newfs()
{
    //rm ./filesys if it exists
    if (remove("filesys") == 0)
    {
        printf("Deleted old filesys\n");
    }
    else
    {
        printf("No old filesys to delete\n");
    }
    makefs("filesys", 1);
}


int main()
{

    makefs("filesys", 1);
    printf("filesys created\n");
    print_inodes();
    ls();
    printf("test1\n");    
    test1();
    print_inodes();
    printf("test1.5\n");
    ls();

    // read, should be empty
    unsigned char block[4096];
    bread(0, block);
    CTEST_ASSERT(block[0] == 0, "block[0] == 0");

    // write
    block[0] = 1;
    bwrite(0, block);

    // read, should be 1
    bread(0, block);
    CTEST_ASSERT(block[0] == 1, "block[0] == 1");

    //test2();

    test2andahalf();
    
    test3();

    test4();
    
    test5();

    test6();

    ls();

    closefs();

    CTEST_RESULTS();

    CTEST_EXIT();
}

#else

int main()
{

    image_open("filesys", 1);

    int inode_num = ialloc();
    printf("inode is %d\n", inode_num);

    int block_num = alloc();
    printf("block is %d\n", block_num);

    unsigned char block[4096];

    bread(0, block);

    block[0] = 1;

    bwrite(0, block);

    bread(0, block);

    image_close();

    return 0;
}

#endif