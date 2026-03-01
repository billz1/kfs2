#pragma once
#include "types.h"

void gdt_install(void);

// Assembly: load GDT from pointer (limit/base)
void load_gdt(void* gdt_ptr);
