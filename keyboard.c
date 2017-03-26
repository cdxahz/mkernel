#include "keyboard.h"
#include "klib.h"
#include "int.h"
#include "keymap.h"
#include "dsr.h"
#include "keyboard_map.h"

#define ENTER_KEY_CODE 0x1C


static int shift_on = 0;
static int ctrl_on = 0;
static int alt_on = 0;

extern unsigned char keyboard_map[128];
extern void load_idt(unsigned long *idt_ptr);
extern void keyboard_handler(void);

struct IDT_entry {
	unsigned short int offset_lowerbits;
	unsigned short int selector;
	unsigned char zero;
	unsigned char type_attr;
	unsigned short int offset_higherbits;
};

struct IDT_entry IDT[IDT_SIZE];


static void kb_dsr(void* param);

void kb_init()
{
	printf("kb_init...\n");
	int_register(0x21, kb_process, 0, 0); 
}

void kb_process(intr_frame *frame)
{
	dsr_add(kb_dsr, 0);
}

static void kb_dsr(void* param)
{
	unsigned char c = read_port(KB_DATA);

	int key_down = 0;
	key_down = ((c & KB_UP_MASK) == 0);
	c &=  (~KB_UP_MASK);
	if (c == KEY_SHIFT){
		if (key_down)
			shift_on = 1;
		else
			shift_on = 0;
		return;
	}

	
	if (c == KEY_CTRL){
		if (key_down)
			ctrl_on = 1;
		else
			ctrl_on = 0;
		return;
	} 
	
	if (c == KEY_ALT){
		if (key_down)
			alt_on = 1;
		else
			alt_on = 0;
		return;
	} 

	if(key_down){
		printf("%c", keyboard_map[(unsigned char) c]);
	}

	// FIXME, file a DSR instead of doing at this context
	// This is a temp way to shutdown, in order to debug through ssh
	static q_count = 0;
	static w_count = 0;
	if (c == KEY_Q && key_down)
		q_count++;
	else if (c == KEY_W && key_down)
		w_count++;
	
	if(c != KEY_Q && key_down)
		q_count = 0;
	
	if(c != KEY_W && key_down)
		w_count = 0;

	if ( q_count >= 10 ){
		shutdown();
	}

	if (w_count >= 10){
		reboot();
	}
}

