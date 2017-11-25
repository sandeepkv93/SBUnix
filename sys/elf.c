#include <sys/alloc.h>
#include <sys/elf64.h>
#include <sys/interrupts.h>
#include <sys/kprintf.h>
#include <sys/string.h>
#include <sys/tarfs.h>
#include <sys/task.h>
#include <sys/vfs.h>
#include <sys/vma.h>
#define X 1
#define W 2
#define R 4

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
    new->vma_filepath = filepath;
    new->vma_file_offset = offset;
    new->vma_flags = flags;
    new->vma_file_size = size;
    new->vma_next = NULL;
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

    for (i = 0; i < ph_num; i++) {
        // TODO: make this PCB->vma_first
        vma_first = vma_add_node(
          vma_first, phdr[i].p_vaddr, phdr[i].p_vaddr + phdr[i].p_memsz,
          filepath, phdr[i].p_filesz, phdr[i].p_offset, phdr[i].p_flags);
    }
    return vma_first;
}
