#if defined(__i386__)

        .code32
        .text
        .global saveContext
        .global restoreContext

saveContext:
        mov 4(%esp), %ecx
        mov %ebx,0x00(%ecx)
        mov %esi,0x04(%ecx)
        mov %edi,0x08(%ecx)
        mov %ebp,0x0c(%ecx)
        lea 4(%esp),%eax
        mov %eax,0x10(%ecx)
        mov (%esp),%eax
        mov %eax,0x14(%ecx)
        xor %eax,%eax
        ret

restoreContext:
        mov 8(%esp), %eax
        mov 4(%esp), %ecx
        mov 0x00(%ecx),%ebx
        mov 0x04(%ecx),%esi
        mov 0x08(%ecx),%edi
        mov 0x0c(%ecx),%ebp
        mov 0x10(%ecx),%esp
        mov 0x14(%ecx),%ecx
        jmp *%ecx

#elif defined(__x86_64__)

        .code64
        .text
        .global saveContext
        .global restoreContext

saveContext:
        mov %rbx,0x00(%rdi)
        mov %r12,0x08(%rdi)
        mov %r13,0x10(%rdi)
        mov %r14,0x18(%rdi)
        mov %r15,0x20(%rdi)
        mov %rbp,0x28(%rdi)
        lea 8(%rsp),%rax
        mov %rax,0x30(%rdi)
        mov (%rsp),%rax
        mov %rax,0x38(%rdi)
        xor %rax,%rax
        ret

restoreContext:
        mov 0x00(%rdi),%rbx
        mov 0x08(%rdi),%r12
        mov 0x10(%rdi),%r13
        mov 0x18(%rdi),%r14
        mov 0x20(%rdi),%r15
        mov 0x28(%rdi),%rbp
        mov 0x38(%rdi),%rcx
        mov 0x30(%rdi),%rsp
        mov %rsi,%rax
        jmpq *%rcx

#else

#error "Implemented for i386 and x86_64 only"
        
#endif
