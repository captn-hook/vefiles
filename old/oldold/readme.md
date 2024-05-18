# VVSFS

### Very Very Simple File System

### Capabilities

Create the file for holding the simulated file system.
Read a block.
Write a block.
Write tests to verify everything is working.

# Building

    make

# Files

[image.c](image.c) - Opens/Closes the file system file, has global int image_fd;

[image.h](image.h)

[block.c](block.c) - Reads/Writes blocks from the file system file

[block.h](block.h)

[testfs.c](testfs.c) - Tests the file system with ctest.h

### Test

[ctest.h](ctest.h) - Testing framework with CTEST_ASSERT, CTEST_RESULTS CTEST_VERBOSE, CTEST_COLOR, CTEST_EXIT

# Data
 
### Reserved Blocks in this File System

    Block  0: Superblock
    Block  1: inode map
    Block  2: Block map
    Block  3: inode data block 0
    Block  4: inode data block 1
    Block  5: inode data block 2
    Block  6: inode data block 3
    Block 7+: File data blocks

# Functions

Open the file system file.

    int image_open(const char *filename, int truncate)
        O_RDWR, O_CREAT, O_TRUNC

Close the file system file.

    int image_close(void)

### Built in

use Unix syscalls like open(), close(), read(), write(), and lseek().

# Tests

# Notes