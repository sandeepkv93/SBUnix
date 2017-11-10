.globl switch_to, switch_to_userland
 switch_to_userland:
    cli
    pop %r8
    /*
    mov $0x23, %ax
    mov %ax, %ds
    mov %ax, %es  
    mov %ax, %fs  
    mov %ax, %gs 
    */
    mov %rsp, %rax
    push $0x23
    push %rax
    pushf
    push $0x1B
    push %r8
    iretq

 switch_to:
// rdi: me
// rsi: next
    movq %rbp,  0(%rdi)
    movq %rax,  8(%rdi)
    movq %rbx, 16(%rdi)
    movq %rcx, 24(%rdi)
    movq %rdx, 32(%rdi)

    pushf 
    pop 48(%rdi)

    push %rdi
    movq %rsp, 40(%rdi)

    //======== Epic Magic

    movq 40(%rsi), %rsp
    pop %rdi

    push 48(%rdi)
    popf

    movq  0(%rdi), %rbp
    movq  8(%rdi), %rax
    movq 16(%rdi), %rbx
    movq 24(%rdi), %rcx
    movq 32(%rdi), %rdx

    retq
.end
