bits 32

section .multiboot
align 4
dd 0x1BADB002
dd 0x0
dd -(0x1BADB002 + 0x0)

section .text
global start
extern kmain

start:
  cli
  mov esp, stack_top
  push ebx        ; multiboot info pointer (optional)
  call kmain
.hang:
  hlt
  jmp .hang

section .bss
align 16
stack_bottom:
  resb 16384
stack_top:
