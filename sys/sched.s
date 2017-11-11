.globl sched_switch_kthread, sched_enter_ring3
 sched_enter_ring3:
 // rdi : rsp3
 // rsi : __start
    cli
    pop %r8
    /*
    mov $0x23, %ax
    mov %ax, %ds
    mov %ax, %es  
    mov %ax, %fs  
    mov %ax, %gs 
    */
    mov %rdi, %rax
    push $0x23   // Pushing SS
    push %rax    // Push next RSP 
    pushf        // Push flags
    //pop %rax
    //or $0x200, %rax
    //push %rax
    push $0x1B   // Push CS
    push %rsi     // Push return address
    iretq

 sched_switch_kthread:
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
