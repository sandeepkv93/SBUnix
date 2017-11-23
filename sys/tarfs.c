#include <sys/ahci.h>
#include <sys/defs.h>
#include <sys/gdt.h>
#include <sys/interrupts.h>
#include <sys/kprintf.h>
#include <sys/nary.h>
#include <sys/pci.h>
#include <sys/string.h>
#include <sys/tarfs.h>
#include <sys/task.h>
#include <sys/utility.h>
#include <sys/vma.h>

void
walk_through_tarfs(char* tarfs_start_address)
{
    struct posix_header_ustar* tarfs_structure =
      (struct posix_header_ustar*)tarfs_start_address;
    int offset = 0;
    uint64_t size;
    while (1) {
        tarfs_structure =
          (struct posix_header_ustar*)(tarfs_start_address + offset);
        if (strlen(tarfs_structure->name) == 0)
            break;
        char node_id[100];
        if ((tarfs_structure->name)[strlen(tarfs_structure->name) - 1] == '/') {
            strcpy(node_id, tarfs_structure->name);
            node_id[strlen(node_id) - 1] = '\0';
        } else {
            strcpy(node_id, strrchr(tarfs_structure->name, '/') + 1);
        }
        struct fs_node_entry tarfs_node;
        strcpy(tarfs_node.node_id, node_id);
        strcpy(tarfs_node.name, tarfs_structure->name);
        strcpy(tarfs_node.mode, tarfs_structure->mode);
        strcpy(tarfs_node.uid, tarfs_structure->uid);
        strcpy(tarfs_node.gid, tarfs_structure->gid);
        strcpy(tarfs_node.size, tarfs_structure->size);
        strcpy(tarfs_node.mtime, tarfs_structure->mtime);
        strcpy(tarfs_node.checksum, tarfs_structure->checksum);
        strcpy(tarfs_node.typeflag, tarfs_structure->typeflag);
        strcpy(tarfs_node.linkname, tarfs_structure->linkname);
        strcpy(tarfs_node.magic, tarfs_structure->magic);
        strcpy(tarfs_node.version, tarfs_structure->version);
        strcpy(tarfs_node.uname, tarfs_structure->uname);
        strcpy(tarfs_node.gname, tarfs_structure->gname);
        strcpy(tarfs_node.devmajor, tarfs_structure->devmajor);
        strcpy(tarfs_node.devminor, tarfs_structure->devminor);
        strcpy(tarfs_node.prefix, tarfs_structure->prefix);
        strcpy(tarfs_node.pad, tarfs_structure->pad);
        tarfs_node.fs_type = 0;
        insert_into_nary_tree(tarfs_node);
        size = octal_to_decimal(char_array_to_int(tarfs_structure->size));
        /*kprintf("Name: %s    Size:%s Address:%p\n", tarfs_structure->name,
                tarfs_structure->size, tarfs_structure);*/
        if (size == 0)
            offset = offset + sizeof(struct posix_header_ustar);
        else {
            if (size % sizeof(struct posix_header_ustar) == 0) {
                offset += size + sizeof(struct posix_header_ustar);
            } else {
                offset += size + (sizeof(struct posix_header_ustar) -
                                  size % sizeof(struct posix_header_ustar)) +
                          sizeof(struct posix_header_ustar);
            }
        }
    }
}
