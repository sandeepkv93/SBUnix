#include <stdarg.h>
#include <string.h>
#include <sys/ahci.h>
#include <sys/defs.h>
#include <sys/gdt.h>
#include <sys/interrupts.h>
#include <sys/kprintf.h>
#include <sys/pci.h>
#include <sys/tarfs.h>
#include <sys/task.h>
#include <sys/vma.h>

uint64_t power (uint64_t x, int e) {
    if (e == 0) return 1;

    return x * power(x, e-1);
}

uint64_t octal_to_decimal(uint64_t octal)
{
    uint64_t decimal = 0, i=0;
    while(octal!=0){
        decimal = decimal + (octal % 10) * power(8,i++);
        octal = octal/10;
    }   
    return decimal;
}

uint64_t char_array_to_int (char *array)
{    
    uint64_t number = 0;
    int mult = 1;
    int n = strlen(array);
    while (n--)
    {
        if ((array[n] < '0' || array[n] > '9') && array[n] != '-') {
            if (number)
                break;
            else
                continue;
        }
        else {
            number += (array[n] - '0') * mult;
            mult *= 10;
        }
    }
    return number;
}

void walk_through_tarfs(char* tarfs_start_address)
{
    struct posix_header_ustar* tarfs_structure = (struct posix_header_ustar *)tarfs_start_address;
    int offset = 0;
    uint64_t size;
    while(1)
    {
        tarfs_structure = (struct posix_header_ustar *)(tarfs_start_address + offset);
        if(strlen(tarfs_structure->name) == 0)
            break;

        size = octal_to_decimal(char_array_to_int(tarfs_structure->size));
        kprintf("Name: %s\tSize:%s Address:%p\n",tarfs_structure->name,tarfs_structure->size,tarfs_structure);
        if(size == 0)
            offset = offset + 512;
        else {
            if(size%512==0) {
                offset += size + 512;
            }
            else {
                offset += size + (512 - size%512) + 512;
            }
        }
    }
}