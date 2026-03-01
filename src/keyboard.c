#include "keyboard.h"
#include "io.h"

static const char keymap[128] = {
  0, 27,'1','2','3','4','5','6','7','8','9','0','-','=', '\b',
  '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
  'a','s','d','f','g','h','j','k','l',';','\'','`', 0,'\\',
  'z','x','c','v','b','n','m',',','.','/', 0,'*',0,' ',
  // rest zeros
};

static int shift = 0;

static char apply_shift(char c) {
  if (c >= 'a' && c <= 'z') return (char)(c - 32);
  switch (c) {
    case '1': return '!'; case '2': return '@'; case '3': return '#';
    case '4': return '$'; case '5': return '%'; case '6': return '^';
    case '7': return '&'; case '8': return '*'; case '9': return '(';
    case '0': return ')'; case '-': return '_'; case '=': return '+';
    case '[': return '{'; case ']': return '}'; case '\\': return '|';
    case ';': return ':'; case '\'': return '"'; case ',': return '<';
    case '.': return '>'; case '/': return '?'; case '`': return '~';
    default: return c;
  }
}

void keyboard_init(void) {
  shift = 0;
}

int keyboard_getchar_nonblock(void) {
  // status port 0x64 bit0: output buffer status
  if ((inb(0x64) & 0x01) == 0) return -1;
  uint8_t sc = inb(0x60);

  if (sc == 0x2A || sc == 0x36) { shift = 1; return -1; } // shift down
  if (sc == 0xAA || sc == 0xB6) { shift = 0; return -1; } // shift up

  if (sc & 0x80) return -1; // key release
  if (sc >= 128) return -1;
  char c = keymap[sc];
  if (!c) return -1;
  if (shift) c = apply_shift(c);
  return (int)(unsigned char)c;
}
