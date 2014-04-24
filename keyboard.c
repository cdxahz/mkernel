#include "keyboard.h"
#include "klib.h"
#include "int.h"
#include "keymap.h"
#include "dsr.h"

static int shift_on = 0;
static int ctrl_on = 0;
static int alt_on = 0;

static void kb_dsr(void* param);

void kb_init()
{
	write_port( 0x21 , 0xFD ) ;
}

void kb_process()
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

	// FIXME, file a DSR instead of doing at this context
	// This is a temp way to shutdown, in order to debug through ssh
	static q_count = 0;
	if (c == KEY_Q && key_down)
		q_count++;
	else if(c != KEY_Q)
		q_count = 0;

	if ( q_count >= 10 ){
		// shutdown
		klib_print("shutdown\n");
		shutdown();
	}
}

