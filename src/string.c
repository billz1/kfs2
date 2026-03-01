#include "string.h"

size_t kstrlen(const char* s) {
  size_t n = 0;
  while (s && s[n]) n++;
  return n;
}

int kstrcmp(const char* a, const char* b) {
  size_t i = 0;
  while (a[i] && b[i]) {
    if (a[i] != b[i]) return (unsigned char)a[i] - (unsigned char)b[i];
    i++;
  }
  return (unsigned char)a[i] - (unsigned char)b[i];
}

void* kmemcpy(void* dst, const void* src, size_t n) {
  uint8_t* d = (uint8_t*)dst;
  const uint8_t* s = (const uint8_t*)src;
  for (size_t i = 0; i < n; i++) d[i] = s[i];
  return dst;
}

void* kmemset(void* dst, int v, size_t n) {
  uint8_t* d = (uint8_t*)dst;
  for (size_t i = 0; i < n; i++) d[i] = (uint8_t)v;
  return dst;
}
