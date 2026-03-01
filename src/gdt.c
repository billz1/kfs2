#include "gdt.h"
#include "string.h"
#include "kprintf.h"

struct __attribute__((packed)) gdt_entry {
  uint16_t limit_low;
  uint16_t base_low;
  uint8_t  base_mid;
  uint8_t  access;
  uint8_t  gran;
  uint8_t  base_hi;
};

struct __attribute__((packed)) gdt_ptr {
  uint16_t limit;
  uint32_t base;
};

static struct gdt_entry gdt_local[7];

static void gdt_set(int idx, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
  gdt_local[idx].limit_low = (uint16_t)(limit & 0xFFFF);
  gdt_local[idx].base_low  = (uint16_t)(base & 0xFFFF);
  gdt_local[idx].base_mid  = (uint8_t)((base >> 16) & 0xFF);
  gdt_local[idx].access    = access;
  gdt_local[idx].gran      = (uint8_t)(((limit >> 16) & 0x0F) | (gran & 0xF0));
  gdt_local[idx].base_hi   = (uint8_t)((base >> 24) & 0xFF);
}

void gdt_install(void) {
  // Null
  gdt_set(0, 0, 0, 0, 0);

  // Flat segments (base 0, limit 4GB)
  // Kernel code: 0x08
  gdt_set(1, 0, 0xFFFFF, 0x9A, 0xCF);
  // Kernel data: 0x10
  gdt_set(2, 0, 0xFFFFF, 0x92, 0xCF);
  // Kernel stack: 0x18 (data segment, separate selector)
  gdt_set(3, 0, 0xFFFFF, 0x92, 0xCF);

  // User code: 0x20 (DPL=3)
  gdt_set(4, 0, 0xFFFFF, 0xFA, 0xCF);
  // User data: 0x28
  gdt_set(5, 0, 0xFFFFF, 0xF2, 0xCF);
  // User stack: 0x30
  gdt_set(6, 0, 0xFFFFF, 0xF2, 0xCF);

  // Subject requirement: GDT must be set at address 0x00000800
  kmemcpy((void*)0x00000800, gdt_local, sizeof(gdt_local));

  struct gdt_ptr gp;
  gp.limit = (uint16_t)(sizeof(gdt_local) - 1);
  gp.base  = 0x00000800;

  load_gdt(&gp);

  kprintf("[gdt] loaded from 0x%p (entries=%u)\n", (void*)gp.base, (unsigned) (sizeof(gdt_local)/sizeof(gdt_local[0])));
}
