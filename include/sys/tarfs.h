#ifndef _TARFS_H
#define _TARFS_H

extern char _binary_tarfs_start;
extern char _binary_tarfs_end;

struct posix_header_ustar
{
    char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char checksum[8];
    char typeflag[1];
    char linkname[100];
    char magic[6];
    char version[2];
    char uname[32];
    char gname[32];
    char devmajor[8];
    char devminor[8];
    char prefix[155];
    char pad[12];
};

uint64_t char_array_to_int (char *array);
uint64_t octal_to_decimal(uint64_t octal);
uint64_t power (uint64_t x, int e);
void walk_through_tarfs(char* tarfs_start_address);

#endif
