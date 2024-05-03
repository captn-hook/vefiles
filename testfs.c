#include <stdio.h>
#include "ctest.h"
#include "image.h"
#include "block.h"

#ifdef CTEST_ENABLE

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