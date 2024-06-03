#include "pack.h"

unsigned int read_u32(void *addr)
{
    unsigned char *bytes = addr;

    return (bytes[0] << 24) |
        (bytes[1] << 16) |
        (bytes[2] << 8) |
        (bytes[3] << 0);
}

unsigned short read_u16(void *addr)
{
    unsigned char *bytes = addr;

    return (bytes[0] << 8) | (bytes[1] << 0);
}

unsigned char read_u8(void *addr)
{
    unsigned char *bytes = addr;

    return bytes[0];
}

void write_u32(void *addr, unsigned long value)
{
    unsigned char *bytes = addr;

    bytes[0] = (value >> 24) & 0xff;
    bytes[1] = (value >> 16) & 0xff;
    bytes[2] = (value >> 8) & 0xff;
    bytes[3] = value & 0xff;
}

void write_u16(void *addr, unsigned int value)
{
    unsigned char *bytes = addr;

    bytes[0] = (value >> 8) & 0xff;
    bytes[1] = value & 0xff;
}

void write_u8(void *addr, unsigned char value)
{
    unsigned char *bytes = addr;

    bytes[0] = value;
}

void pack_inode(void *addr, struct inode *in) {
    write_u32(addr, in->size); 
    write_u16(addr + 4, in->owner_id);
    write_u8(addr + 6, in->permissions);
    write_u8(addr + 7, in->flags);
    write_u8(addr + 8, in->link_count);
    for (int i = 0; i < INODE_PTR_COUNT; i++) { // always writes 16 block pointers
        write_u16(addr + 9 + i*2, in->block_ptr[i]);
    }

    printf("Packed inode: size: %d, owner_id: %d, permissions: %d, flags: %d, link_count: %d\n", in->size, in->owner_id, in->permissions, in->flags, in->link_count);
}

void unpack_inode(void *addr, struct inode *in) {
    in->size = read_u32(addr);
    in->owner_id = read_u16(addr + 4); // 4 because 32 bit int is 4 bytes
    in->permissions = read_u8(addr + 6); // and 8 bit int is 1 byte
    in->flags = read_u8(addr + 7);
    in->link_count = read_u8(addr + 8);
    for (int i = 0; i < INODE_PTR_COUNT; i++) {
        in->block_ptr[i] = read_u16(addr + 9 + i*2); // maffs
    }

    printf("Unpacked inode: size: %d, owner_id: %d, permissions: %d, flags: %d, link_count: %d\n", in->size, in->owner_id, in->permissions, in->flags, in->link_count);
}
