.globl timer_isr_asm 
.extern timer_handler 
push_regs:
    push %rsi
    push %rdi
    push %rbp
    push %rax
    push %rbx
    push %rcx
    push %rdx
    jmp %r10

pop_regs:
   pop %rdx
   pop %rcx
   pop %rbx
   pop %rax
   pop %rbp
   pop %rdi
   pop %rsi
   jmp %r10

//timer_isr_asm1:
//   jmp push_regs
//push_ret:
//   callq timer_handler
//   jmp pop_regs
//pop_ret:
//   iretq

timer_isr_asm:   
   movq timer_push_ret, %r10
   jmp  push_regs
timer_push_ret:
   callq timer_handler
   movq timer_pop_ret, %r10
timer_pop_ret:
   iretq

/*
timer_isr_asm:
   movq %pc, %r10
   jmp push_regs
push_ret:
   callq timer_handler
   movq %pc, %r10
   jmp pop_regs
pop_ret:
   iretq
*/
//kb_isr_asm:
//    jmp push_regs
//push_ret_kb:
.end
