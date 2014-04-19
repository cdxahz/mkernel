#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_
#define KB_DATA 0x60
#define KB_UP_MASK 0x80

void kb_init();
void kb_process();
#endif
