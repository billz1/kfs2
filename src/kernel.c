#include "terminal.h"
#include "kprintf.h"
#include "keyboard.h"
#include "gdt.h"
#include "stackdump.h"
#include "shell.h"

void kmain(void* mb_info) {
  (void)mb_info;

  term_init();
  term_setcolor(VGA_WHITE, VGA_BLACK);
  kprintf("KFS2 starting...\n");

  // Mandatory: display 42 somewhere
  term_setcolor(VGA_LIGHT_GREEN, VGA_BLACK);
  kprintf("42\n");
  term_setcolor(VGA_LIGHT_GREY, VGA_BLACK);

  // Mandatory: GDT with required segments and located at 0x800
  gdt_install();

  // Mandatory: stack dump tool (human friendly)
  stack_dump(16);

  // Bonus: shell using keyboard input
  keyboard_init();
  shell_run();

  for(;;) __asm__ volatile("hlt");
}
