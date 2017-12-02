#include <sys/alloc.h>
#include <sys/elf64.h>
#include <sys/interrupts.h>
#include <sys/kprintf.h>
#include <sys/paging.h>
#include <sys/string.h>
#include <sys/string.h>
#include <sys/tarfs.h>
#include <sys/task.h>
#include <sys/vfs.h>
#include <sys/vma.h>
#define X 1
#define W 2
#define R 4
#define VMA_STACK_END (PAGING_KERNMEM)
#define VMA_STACK_START (VMA_STACK_END - (500 * PAGING_PAGE_SIZE))
#define VMA_HEAP_START (VMA_STACK_END - (6000 * PAGING_PAGE_SIZE))

void
elf_read(char* binary_file)
{
    Elf64_Ehdr ehdr;
    Elf64_Phdr* phdr;

    int fd = vfs_open(binary_file, 0);

    vfs_read(fd, &ehdr, sizeof(Elf64_Ehdr));

    // Set entry point and binary name in current task_struct
    task_get_this_task_struct()->entry_point = ehdr.e_entry;
    strcpy(task_get_this_task_struct()->binary_name, binary_file);

    phdr = (Elf64_Phdr*)kmalloc(sizeof(Elf64_Phdr) * ehdr.e_phnum);
    // TODO: load only LOAD and STACK and whichever is needed :/
    vfs_seek(fd, ehdr.e_phoff);

    for (int i = 0; i < ehdr.e_phnum; i++) {
        vfs_read(fd, &phdr[i], sizeof(Elf64_Phdr));
        vfs_seek(fd, ehdr.e_phoff + (i + 1) * ehdr.e_phentsize);
    }

    // Create the vmas for current process
    task_get_this_task_struct()->vma_list =
      vma_list_with_phdr(NULL, phdr, ehdr.e_phnum, binary_file);

    vfs_close(fd);
}

struct vma_struct*
vma_add_node(struct vma_struct* vma_first, uint64_t start, uint64_t end,
             char* filepath, uint64_t size, uint64_t offset, uint32_t flags)
{
    // TODO: add elements in the increasing order of the virtual addresses.
    // Handle merging and splitting
    struct vma_struct* new, *list_iter;
    new = (struct vma_struct*)kmalloc(sizeof(struct vma_struct));
    new->vma_start = start;
    new->vma_end = end;
    // strcpy(new->vma_filepath, filepath);
    new->vma_file_offset = offset;
    new->vma_flags = flags;
    new->vma_file_size = size;
    new->vma_next = NULL;
    new->vma_type = VMA_LOAD;
    if (vma_first == NULL) {
        // TODO: rewrite prototypes once the PCB struct is known
        return new;
    }
    list_iter = vma_first;
    while (list_iter->vma_next != NULL) {
        list_iter = list_iter->vma_next;
    }
    list_iter->vma_next = new;
    return vma_first;
}

struct vma_struct*
vma_list_with_phdr(struct vma_struct* vma_first, Elf64_Phdr* phdr,
                   uint16_t ph_num, char* filepath)
{
    int i = 0;
    struct vma_struct *stack, *heap;
    struct vma_struct* vma_temp;
    for (i = 0; i < ph_num; i++) {
        // TODO: make this PCB->vma_first
        vma_first = vma_add_node(
          vma_first, phdr[i].p_vaddr, phdr[i].p_vaddr + phdr[i].p_memsz,
          filepath, phdr[i].p_filesz, phdr[i].p_offset, phdr[i].p_flags);
    }
    vma_temp = vma_first;
    while (vma_temp->vma_next != NULL)
        vma_temp = vma_temp->vma_next;

    stack = (struct vma_struct*)kmalloc(sizeof(struct vma_struct));
    stack->vma_start = VMA_STACK_START;
    stack->vma_end = VMA_STACK_END;
    // stack->vma_filepath = '\0';
    stack->vma_file_offset = 0;
    stack->vma_flags = R | W;
    stack->vma_file_size = 0;
    stack->vma_next = NULL;
    stack->vma_type = VMA_STACK;

    vma_temp->vma_next = stack;
    vma_temp = stack;

    heap = (struct vma_struct*)kmalloc(sizeof(struct vma_struct));
    heap->vma_start = heap->vma_end = VMA_HEAP_START;
    // heap->vma_filepath = '\0';
    heap->vma_file_offset = 0;
    heap->vma_flags = R | W;
    heap->vma_file_size = 0;
    heap->vma_next = NULL;
    heap->vma_type = VMA_HEAP;

    vma_temp->vma_next = heap;

    return vma_first;
}
