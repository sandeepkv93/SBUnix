.globl timer_isr_asm, kb_isr_asm
.extern timer_isr, kb_isr 

push_regs:
    pop %r8
    push %rsi
    push %rdi
    push %rbp
    push %rax
    push %rbx
    push %rcx
    push %rdx
    push %r8
    retq

pop_regs:
    pop %r8
    pop %rdx
    pop %rcx
    pop %rbx
    pop %rax
    pop %rbp
    pop %rdi
    pop %rsi
    push %r8
    retq

timer_isr_asm:
    callq push_regs
    callq timer_isr
    callq pop_regs
    iretq

kb_isr_asm:
    callq push_regs
    callq kb_isr
    callq pop_regs
    iretq

.end
