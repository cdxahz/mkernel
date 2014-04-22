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

// following is above 0xC0000000 code & data
#define KHEAP_END	 0xC0800000
#define KHEAP_BEGIN	 0xC0300000
#define NULL (void*)0
typedef struct _kblock
{
	unsigned int size;
	struct _kblock *next;
}kblock;

void* kmalloc(unsigned int size);

void kfree(void* buf);
#endif
