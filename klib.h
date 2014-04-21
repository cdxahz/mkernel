#ifndef _KLIB_H_
#define _KLIB_H_
#include "mm.h"

_START void klib_init();

_START void klib_putchar(char c);

_START void klib_putint(int num);

_START void klib_print(char *str);

_START void klib_info(char *info, int num, char* end);

_START void klib_clear();

_START char* klib_itoa(char* str, int num);

#endif
