#include "kprintf.h"
#include "terminal.h"
#include <stdarg.h>

static void put_u32_hex(uint32_t v, int upper) {
  char buf[8];
  for (int i = 7; i >= 0; i--) {
    uint8_t d = (uint8_t)(v & 0xF);
    if (d < 10) buf[i] = (char)('0' + d);
    else buf[i] = (char)((upper ? 'A' : 'a') + (d - 10));
    v >>= 4;
  }
  for (int i = 0; i < 8; i++) term_putc(buf[i]);
}

static void put_u32_dec(uint32_t v) {
  char buf[11];
  int i = 0;
  if (v == 0) { term_putc('0'); return; }
  while (v && i < 10) { buf[i++] = (char)('0' + (v % 10)); v /= 10; }
  for (int j = i - 1; j >= 0; j--) term_putc(buf[j]);
}

static void put_i32_dec(int32_t v) {
  if (v < 0) { term_putc('-'); put_u32_dec((uint32_t)(-v)); }
  else put_u32_dec((uint32_t)v);
}

int kprintf(const char* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int out = 0;

  for (const char* p = fmt; p && *p; p++) {
    if (*p != '%') { term_putc(*p); out++; continue; }
    p++;
    if (!*p) break;
    switch (*p) {
      case '%': term_putc('%'); out++; break;
      case 'c': term_putc((char)va_arg(ap, int)); out++; break;
      case 's': {
        const char* s = va_arg(ap, const char*);
        if (!s) s = "(null)";
        while (*s) { term_putc(*s++); out++; }
        break;
      }
      case 'd': put_i32_dec(va_arg(ap, int32_t)); break;
      case 'u': put_u32_dec(va_arg(ap, uint32_t)); break;
      case 'x': put_u32_hex(va_arg(ap, uint32_t), 0); break;
      case 'X': put_u32_hex(va_arg(ap, uint32_t), 1); break;
      case 'p': term_write("0x"); put_u32_hex((uint32_t)(uintptr_t)va_arg(ap, void*), 0); break;
      default:
        term_putc('%'); term_putc(*p); out += 2;
        break;
    }
  }
  va_end(ap);
  return out;
}
