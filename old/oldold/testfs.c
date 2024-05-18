#include <stdio.h>
#include "ctest.h"
#include "image.h"
#include "block.h"

#ifdef CTEST_ENABLE

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


int main() {

    image_open("filesys", 1);

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

    unsigned char block[4096];
    
    bread(0, block);

    block[0] = 1;

    bwrite(0, block);

    bread(0, block);

    image_close();

    return 0;
}

#endif