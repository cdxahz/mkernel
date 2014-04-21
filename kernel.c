/*
* Copyright (C) 2014  Arjun Sreedharan
* License: GPL version 2 or higher http://www.gnu.org/licenses/gpl.html
*/
#include "klib.h"
#include "int.h"
#include "keyboard.h"
#include "mm.h"
#include "multiboot.h"


_START static void run(void);
_START static void init(multiboot_info_t* mb);

_START void kmain(multiboot_info_t* mb, unsigned int magic)
{

    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        return;
    }


    init(mb);


    run();
    
    // init(mb); 
	// run();
	return;
}



_START static void init(multiboot_info_t* mb)
{

    klib_init();

    int_init();

    return;

    mm_init(mb);

    // Here we can access all virtual memory
	kb_init();
}

_START static void run()
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
