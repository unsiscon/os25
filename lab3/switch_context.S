.global switch_context

# switch_context(old_context, new_context)
# rdi: old_context pointer
# rsi: new_context pointer

switch_context:
    # Save old context (callee-saved registers)
    movq %rsp, 0x00(%rdi)
    movq %r15, 0x08(%rdi)
    movq %r14, 0x10(%rdi)
    movq %r13, 0x18(%rdi)
    movq %r12, 0x20(%rdi)
    movq %rbx, 0x28(%rdi)
    movq %rbp, 0x30(%rdi)

    # Load new context
    movq 0x00(%rsi), %rsp
    movq 0x08(%rsi), %r15
    movq 0x10(%rsi), %r14
    movq 0x18(%rsi), %r13
    movq 0x20(%rsi), %r12
    movq 0x28(%rsi), %rbx
    movq 0x30(%rsi), %rbp

    ret