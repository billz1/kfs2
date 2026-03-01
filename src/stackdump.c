#include "stackdump.h"
#include "kprintf.h"

void stack_dump(uint32_t dwords) {
  uint32_t esp, ebp;
  __asm__ volatile("mov %%esp, %0" : "=r"(esp));
  __asm__ volatile("mov %%ebp, %0" : "=r"(ebp));

  kprintf("---- stack dump ----\n");
  kprintf("ESP=0x%p  EBP=0x%p\n", (void*)esp, (void*)ebp);

  uint32_t* p = (uint32_t*)esp;
  for (uint32_t i = 0; i < dwords; i++) {
    kprintf("0x%p : 0x%08x\n", (void*)(uintptr_t)(esp + i*4), p[i]);
  }
  kprintf("--------------------\n");
}
