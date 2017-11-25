#ifndef _ELF64_H
#define _ELF64_H

#define EI_NIDENT 16

typedef uint64_t Elf64_Addr;
typedef uint16_t Elf64_Half;
typedef uint64_t Elf64_Lword;
typedef uint64_t Elf64_Off;
typedef uint32_t Elf64_Sword;
typedef uint64_t Elf64_Sxword;
typedef uint32_t Elf64_Word;
typedef uint64_t Elf64_Xword;

typedef struct
{
    unsigned char e_ident[EI_NIDENT];
    Elf64_Half e_type;      // 64 or 32
    Elf64_Half e_machine;   // little or big endian
    Elf64_Word e_version;   // ELF version
    Elf64_Addr e_entry;     // program entry position
    Elf64_Off e_phoff;      // program header table position
    Elf64_Off e_shoff;      // section header table position
    Elf64_Word e_flags;     // arch dependent flags
    Elf64_Half e_ehsize;    // elf header size
    Elf64_Half e_phentsize; // program header entry size
    Elf64_Half e_phnum;     // number of entries in the program header
    Elf64_Half e_shentsize; // size of entry in the section header
    Elf64_Half e_shnum;     // number of entries in the section table
    Elf64_Half
      e_shstrndx; // index in section header table with the section names
} Elf64_Ehdr;

typedef struct
{
    Elf64_Word p_type;  // type of segment
    Elf64_Word p_flags; // flag rwx
    Elf64_Off p_offset; // The offset in the file that the data for this segment
                        // can be found
    Elf64_Addr
      p_vaddr; // Where you should start to put this segment in virtual memory
    Elf64_Addr p_paddr;   //
    Elf64_Xword p_filesz; // Size of the segment in the file
    Elf64_Xword p_memsz;  // Size of the segment in memory
    Elf64_Xword p_align;  // The required alignment for this section
} Elf64_Phdr;

#endif
