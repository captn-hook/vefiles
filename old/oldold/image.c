#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "block.h"

// File descriptor global 
int image_fd = -1;

int image_open(char *filename, int truncate) {
    
    image_fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (image_fd < 0) {
        perror("image_open");
        return -1;
    }

    if (truncate) {
        if (ftruncate(image_fd, 1024*1024) < 0) {
            perror("image_open");
            return -1;
        }
    }

    return 0;
}

int image_close(void) {
    
    if (close(image_fd) < 0) {
        perror("image_close");
        return -1;
    }

    return 0;
}