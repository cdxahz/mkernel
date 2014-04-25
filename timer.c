#include "timer.h"
#include "int.h"

static void timer_process(intr_frame* frame)
{
	// printf("timer process\n");
}



void timer_init()
{
	int_register(0x20, timer_process, 0, 0);
}



