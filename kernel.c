
#include "klib.h"
#include "int.h"
#include "keyboard.h"
#include "mm.h"
#include "multiboot.h"
#include "dsr.h"
#include "timer.h"

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

#ifdef TEST_MALLOC
	static void test_malloc()
	{
#define MIN_MEM 1
#define MAX_MEM 4080
#define ALLOC_COUNT 10
		// test kmalloc, kfree
		void* addr[ALLOC_COUNT];
		int size = 0;
		int mallocor = 10;
		int* a = kmalloc(1);
		int* b = kmalloc(25);
		int i = 0;
		kfree(a);
		kfree(b);
		while (1){
			int mem_count = klib_rand() % ALLOC_COUNT;
			if (mem_count == 0)
				mem_count = 1;
			for (i = 0; i < mem_count; i++){
				size = klib_rand() % (MAX_MEM-MIN_MEM);
				size += MIN_MEM;
				if (size == 0)
					addr[i] = 0;
				else
				{
					addr[i] = kmalloc(size);
					if (addr[i])
						memset(addr[i], 0xc, size);
				}
			}
			for (i = 0; i < mem_count; i++){
				kfree(addr[i]);
			}
		}
	}

#endif
extern unsigned long *virtual_intr_stubs;
extern _STARTDATA unsigned long intr_stubs[IDT_SIZE];
extern unsigned long long *virtual_idt;
void kmain_startup()
{
    int i = 0;
    
	
	int_enable_all();

/*
	printf("intr_stub %x, virtual_intr_stubs %x\n", intr_stubs, virtual_intr_stubs);

	for(i = 0; i < 10; i++)
		printf("intr[%d] %x\n",i, intr_stubs[i] );
	
	for (i = 0; i < 10; i++){
		int *a = (int*)&(virtual_idt[i]);
		printf("idt[i] %x %x\n", a[0], a[1]);
	}
*/
	kb_init();

	timer_init();

    klib_init();
	
    // now we are debuggable
    // printf("hello from %d, %u, %x, %s\n", -100, -100, -100, "world");

#ifdef TEST_MALLOC
	test_malloc();
#endif

	dsr_init();

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

	while(1){
		dsr_process();
    }
}
