#ifndef _VMA_H
#define _VMA_H
#include <sys/elf64.h>
struct vma_struct
{
    uint64_t vma_start; // first addr within range
    uint64_t vma_end;   // first addr outside range
    struct vma_struct* vma_next;
    char* vma_filepath;
    uint64_t vma_file_offset;
    uint32_t vma_flags;
    uint64_t vma_file_size;
};
struct vma_struct* vma_add_node(struct vma_struct* vma_first, uint64_t start,
                                uint64_t end, char* filepath, uint64_t size,
                                uint64_t offset, uint32_t flags);

struct vma_struct* vma_list_with_phdr(struct vma_struct* vma_first,
                                      Elf64_Phdr* phdr, uint16_t ph_num,
                                      char* filepath);
#endif
