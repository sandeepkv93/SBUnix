.globl paging_enable, paging_set

paging_enable:
    push %rbx
    push %rax

    movq %rdi, %cr3
    movq %cr0, %rax

    movq $0x80000001, %rbx
    orq %rbx, %rax
    movq %rax, %cr0

    pop %rax
    pop %rbx

    retq
.end
