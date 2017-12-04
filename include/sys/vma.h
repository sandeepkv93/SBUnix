#ifndef _VMA_H
#define _VMA_H
#include <sys/elf64.h>

#define VMA_LOAD 1
#define VMA_STACK 2
#define VMA_HEAP 4

struct vma_struct
{
    uint64_t vma_start; // first addr within range
    uint64_t vma_end;   // first addr outside range
    struct vma_struct* vma_next;
    //    char vma_filepath[30];
    uint64_t vma_file_offset;
    uint32_t vma_flags; // rwx permissions
    uint64_t vma_file_size;
    uint32_t vma_type; // stack, heap, other types
};
struct vma_struct* vma_add_node(struct vma_struct* vma_first, uint64_t start,
                                uint64_t end, char* filepath, uint64_t size,
                                uint64_t offset, uint32_t flags);

struct vma_struct* vma_list_with_phdr(struct vma_struct* vma_first,
                                      Elf64_Phdr* phdr, uint16_t ph_num,
                                      char* filepath);
bool vma_read_elf(char* filepath);
struct vma_struct* vma_deep_copy_list(struct vma_struct* head);

#endif
