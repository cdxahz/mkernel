#include "timer.h"
#include "int.h"

static unsigned long tickets;
static unsigned long seconds;
static unsigned long minutes;
static unsigned long hourse;
static unsigned long days;

static unsigned long total_seconds;
static void timer_process(intr_frame* frame)
{
	// printf("timer process\n");
    tickets++;
    if ( tickets  == HZ) {
        seconds++;
        total_seconds++;
        tickets = 0;
    }

    if (seconds == 60) {
        minutes ++;
        seconds = 0;
    }

    if (minutes == 60) {
        hourse ++;
        minutes = 0;
    }

    if (hourse == 24) {
        days ++;
        hourse = 0;
    }

    printf("\r%d days %d hourse %d minutes %d seconds", days, hourse, minutes, seconds); 
}



void timer_init()
{
    timer_control control;

    tickets = 0;
    seconds = 0;
    minutes = 0;
    hourse = 0;
    days = 0;

	int_register(0x20, timer_process, 0, 0);

    control.channel = CHANNEL_0;
    control.bcd_mode = BCD_16_DIGIT_BINARY;
    control.access_mode = ACCESS_MODE_BOTH;
    control.operating_mode = OPERATION_MODE_RATE;

    write_port(TIMER_CONTROL_MASK, *((unsigned char *)&control));
    write_port(TIMER_CHANNEL_0, LATCH & 0xff);
    write_port(TIMER_CHANNEL_0, LATCH  >> 8);


}

void timer_current(time_t* time)
{
    time->seconds = total_seconds;
    time->milliseconds = tickets*10;
}



