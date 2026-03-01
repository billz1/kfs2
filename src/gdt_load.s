bits 32
section .text
global load_gdt

load_gdt:
  mov eax, [esp + 4]
  lgdt [eax]

  mov ax, 0x10      ; kernel data selector
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov ss, ax

  mov ax, 0x18      ; kernel "stack" selector (we treat it like data)
  mov gs, ax

  jmp 0x08:.flush   ; far jump to reload CS
.flush:
  ret

section .note.GNU-stack noalloc noexec nowrite progbits
