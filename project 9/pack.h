#ifndef PACK_H
#define PACK_H

#include "block.h"
#include "free.h"

struct inode; 

unsigned int read_u32(void *addr);
unsigned short read_u16(void *addr);
unsigned char read_u8(void *addr);
void write_u32(void *addr, unsigned long value);
void write_u16(void *addr, unsigned int value);
void write_u8(void *addr, unsigned char value);

void pack_inode(void *addr, struct inode *in);
void unpack_inode(void *addr, struct inode *in);

#include "inode.h"

#endif
