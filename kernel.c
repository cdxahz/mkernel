/*
* Copyright (C) 2014  Arjun Sreedharan
* License: GPL version 2 or higher http://www.gnu.org/licenses/gpl.html
*/
#include "klib.h"
#include "int.h"
#include "keyboard.h"
static void run(void);
static void init(void);

void kmain(void)
{
	init();
	run();
	return;
}

static void init()
{
	klib_init();
	int_init();
	kb_init();
}

static void run()
{
	
	int count = 0;
	/*
	while(1){
		klib_print("hello kernel\t");
		klib_putint(count);
		count++;
		if (count > 0xffff)
			count = 0;
	}
	*/
	while(1);
}
