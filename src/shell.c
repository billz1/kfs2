#include "shell.h"
#include "kprintf.h"
#include "keyboard.h"
#include "terminal.h"
#include "io.h"
#include "string.h"
#include "stackdump.h"

static void prompt(void) {
  term_setcolor(VGA_LIGHT_GREEN, VGA_BLACK);
  kprintf("kfs2> ");
  term_setcolor(VGA_LIGHT_GREY, VGA_BLACK);
}

static void exec_cmd(char* buf) {
  // trim leading spaces
  while (*buf == ' ') buf++;

  if (kstrcmp(buf, "help") == 0) {
    kprintf("Commands:\n");
    kprintf("  help        - show this help\n");
    kprintf("  clear       - clear screen\n");
    kprintf("  stack [n]   - dump kernel stack (dwords)\n");
    kprintf("  halt        - halt CPU\n");
    kprintf("  reboot      - reboot (8042)\n");
    kprintf("  echo <txt>  - print text\n");
    return;
  }
  if (kstrcmp(buf, "clear") == 0) { term_clear(); return; }
  if (kstrcmp(buf, "halt") == 0) {
    kprintf("Halting...\n");
    for (;;) __asm__ volatile("hlt");
  }
  if (kstrcmp(buf, "reboot") == 0) {
    kprintf("Rebooting...\n");
    // 8042 reset
    while (inb(0x64) & 0x02) {}
    outb(0x64, 0xFE);
    for (;;) __asm__ volatile("hlt");
  }

  if (buf[0]=='s' && buf[1]=='t' && buf[2]=='a' && buf[3]=='c' && buf[4]=='k') {
    uint32_t n = 16;
    // parse optional number
    char* p = buf + 5;
    while (*p==' ') p++;
    if (*p) {
      uint32_t v = 0;
      while (*p>='0' && *p<='9') { v = v*10 + (uint32_t)(*p - '0'); p++; }
      if (v) n = v;
    }
    stack_dump(n);
    return;
  }

  if (buf[0]=='e' && buf[1]=='c' && buf[2]=='h' && buf[3]=='o' && buf[4]==' ') {
    kprintf("%s\n", buf + 5);
    return;
  }

  if (*buf == 0) return;
  kprintf("Unknown: %s (try 'help')\n", buf);
}

void shell_run(void) {
  char buf[256];
  size_t len = 0;

  kprintf("Type 'help' for commands.\n");
  prompt();

  for (;;) {
    int ch = keyboard_getchar_nonblock();
    if (ch < 0) continue;

    if (ch == '\n') {
      term_putc('\n');
      buf[len] = 0;
      exec_cmd(buf);
      len = 0;
      prompt();
      continue;
    }
    if (ch == '\b') {
      if (len > 0) { len--; term_putc('\b'); }
      continue;
    }
    if (ch == '\t') {
      // treat tab as 2 spaces
      for (int i=0;i<2;i++){
        if (len < sizeof(buf)-1) { buf[len++]=' '; term_putc(' '); }
      }
      continue;
    }

    if (len < sizeof(buf)-1) {
      buf[len++] = (char)ch;
      term_putc((char)ch);
    }
  }
}
