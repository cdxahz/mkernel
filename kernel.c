/*
* Copyright (C) 2014  Arjun Sreedharan
* License: GPL version 2 or higher http://www.gnu.org/licenses/gpl.html
*/
#include "klib.h"
#include "int.h"
#include "keyboard.h"
#include "mm.h"
#include "multiboot.h"


static void run(void);
_START static void init(multiboot_info_t* mb);

_START void kmain(multiboot_info_t* mb, unsigned int magic)
{

    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        return;
    }


    init(mb);


	// never to here
	
	return;
}

void kmain_startup()
{
    int _cr0 = 0;
    klib_init();
	
	kb_init();

    // now we are debuggable
    // printf("hello from %d, %u, %x, %s\n", -100, -100, -100, "world");


    int_diags();

	run();
}



_START static void init(multiboot_info_t* mb)
{


    int_init();

    mm_init(mb);
	
	
	// never to here
	return;

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
	while(1){
        __asm__("nop");
    }
}
