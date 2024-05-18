#include <stdio.h>

#include "ctest.h"

#include "image.h"
#include "block.h"
#include "free.h"
#include "inode.h"

#ifdef CTEST_ENABLE

void test1() {
    int inode_num = ialloc();
    CTEST_ASSERT(inode_num == 0, "inode_num == 0");

    int block_num = alloc();
    CTEST_ASSERT(block_num == 0, "block_num == 0");
}

int test2() {
    //write a large amount of data 
    unsigned char block[4096];
    for (int i = 0; i < 4096; i++) {
        block[i] = i % 256;
    }

    for (int i = 0; i < 1000; i++) {
        bwrite(i, block);
    }

    //read the data back
    int return_value = 0;
    for (int i = 0; i < 1000; i++) {
        bread(i, block);
        for (int j = 0; j < 4096; j++) {
            CTEST_ASSERT(block[j] == j % 256, "block[j] == j % 256");
            if (block[j] != j % 256) {
                return_value = -1;
            }
        }
    }

    return return_value;
}

void test3() {
    
    struct inode *inode = incore_find_free();
    CTEST_ASSERT(inode != NULL, "inode != NULL");

    inode->ref_count = 1;
    inode->flags = 1;

    struct inode *inode2 = incore_find(inode->inode_num);
    CTEST_ASSERT(inode2 != NULL, "inode2 != NULL");
    CTEST_ASSERT(inode2->flags == 1, "inode2->flags == 1");

    incore_free_all();
    CTEST_ASSERT(inode->ref_count == 0, "inode->ref_count == 0");
    CTEST_ASSERT(inode2->ref_count == 0, "inode2->ref_count == 0");
    
}


int main() {

    image_open("filesys", 1);

    test1();

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

    test2();

    image_close();

    CTEST_RESULTS();

    CTEST_EXIT();
}

#else

int main() {
     
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