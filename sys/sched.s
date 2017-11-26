.globl sched_switch_kthread, sched_enter_ring3, sched_fork_wrapper

 sched_enter_ring3:
 // rdi : rsp3 (Ring 3 stack)
 // rsi : __start (Return function in ring 3)

    // We don't want context switch in between
    cli

    // Remove return address from stack
    add $0x8, %rsp

    // Pushing SS, b1 b0 signify ring (11b).
    // b5 b4 b3 signify index of Ring3 data segment (100b)
    push $0x23

    // Push next RSP 
    mov %rdi, %rax
    push %rax

    // Push flags
    pushf

    // We cleverly pop the flags from stack and set the interrupts bit
    // this will enable interrupts when iretq executes
    pop %rax
    or $0x200, %rax
    push %rax

    // Push CS, see SS explanation above for value
    push $0x2B   

    // Push return address
    push %rsi

    iretq

 sched_switch_kthread:
 // rdi: me (pointer to current task_struct)
 // rsi: next (pointer to next task_struct)

    // Save all registers(except RSP) into task_struct
    movq %rbp,  0(%rdi)
    movq %rax,  8(%rdi)
    movq %rbx, 16(%rdi)
    movq %rcx, 24(%rdi)
    movq %rdx, 32(%rdi)
    pushf 
    pop 48(%rdi)

    movq %cr3, %rax
    movq %rax, 56(%rdi)

    // Save 'me' (address of task_struct) on stack
    push %rdi

    // Now save latest RSP as well in the task_stuct.
    movq %rsp, 40(%rdi)


    // Do the steps above in reverse with next task_struct
    // Magic!


    // change RSP
    movq 40(%rsi), %rsp

    // Get task_struct address
    pop %rdi

    movq 56(%rdi), %rax
    movq %rax, %cr3

    // Restore registers
    push 48(%rdi)
    popf
    movq  0(%rdi), %rbp
    movq  8(%rdi), %rax
    movq 16(%rdi), %rbx
    movq 24(%rdi), %rcx
    movq 32(%rdi), %rdx

    retq

sched_fork_wrapper:
    movq %rsp, %rdx
    callq fork_copy_stack
    retq
.end
