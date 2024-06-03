#ifndef IMAGE_H
#define IMAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "block.h"

int image_open(char *filename, int truncate);
int image_close(void);

extern int image_fd;

#endif