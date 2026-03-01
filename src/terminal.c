#include "terminal.h"
#include "io.h"
#include "string.h"

static volatile uint16_t* const VGA = (uint16_t*)0xB8000;
static const int W = 80;
static const int H = 25;

static int row = 0, col = 0;
static uint8_t color = 0x07;
static int cursor_enabled = 1;

static inline uint16_t vga_entry(char c, uint8_t colr) {
  return (uint16_t)c | ((uint16_t)colr << 8);
}

static void hw_cursor_update(void) {
  if (!cursor_enabled) return;
  uint16_t pos = (uint16_t)(row * W + col);
  outb(0x3D4, 0x0F);
  outb(0x3D5, (uint8_t)(pos & 0xFF));
  outb(0x3D4, 0x0E);
  outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void term_set_cursor(int enable) {
  cursor_enabled = enable ? 1 : 0;
  if (!cursor_enabled) {
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
  } else {
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x00);
    hw_cursor_update();
  }
}

static void scroll_if_needed(void) {
  if (row < H) return;
  // move lines up
  for (int r = 1; r < H; r++) {
    for (int c = 0; c < W; c++) {
      VGA[(r-1)*W + c] = VGA[r*W + c];
    }
  }
  // clear last line
  for (int c = 0; c < W; c++) VGA[(H-1)*W + c] = vga_entry(' ', color);
  row = H - 1;
}

void term_init(void) {
  row = 0; col = 0;
  color = 0x07;
  term_clear();
  term_set_cursor(1);
}

void term_setcolor(uint8_t fg, uint8_t bg) {
  color = (uint8_t)((bg << 4) | (fg & 0x0F));
}

void term_clear(void) {
  for (int r = 0; r < H; r++) {
    for (int c = 0; c < W; c++) {
      VGA[r*W + c] = vga_entry(' ', color);
    }
  }
  row = 0; col = 0;
  hw_cursor_update();
}

void term_putc(char ch) {
  if (ch == '\n') {
    col = 0;
    row++;
    scroll_if_needed();
    hw_cursor_update();
    return;
  }
  if (ch == '\r') {
    col = 0;
    hw_cursor_update();
    return;
  }
  if (ch == '\t') {
    int next = (col + 4) & ~3;
    while (col < next) term_putc(' ');
    return;
  }
  if (ch == '\b') {
    if (col > 0) col--;
    else if (row > 0) { row--; col = W-1; }
    VGA[row*W + col] = vga_entry(' ', color);
    hw_cursor_update();
    return;
  }

  VGA[row*W + col] = vga_entry(ch, color);
  col++;
  if (col >= W) {
    col = 0;
    row++;
    scroll_if_needed();
  }
  hw_cursor_update();
}

void term_write(const char* s) {
  for (size_t i = 0; s && s[i]; i++) term_putc(s[i]);
}
