#include "klib.h"
#include "tty.h"


static int cursor = 0;
#define CUR_ROW (cursor / TTY_MAX_COL)
#define CUR_COL (cursor % TTY_MAX_COL)

static void klib_cursor_forward(int new_pos);

void klib_init()
{
	tty_init();

	cursor = 0;
}


_START void klib_putchar(char c)
{
	int new_pos = 0;

	if ( c == '\n'){
		int new_row = CUR_ROW + 1;
		int new_col = 0;
		new_pos = ROW_COL_TO_CUR(new_row, new_col);
	}else if (c == '\t'){
		int loop_count = 4 - (CUR_COL % 4);
		int i = 0;
		if (loop_count == 0)
		  loop_count = 4;
		for (i = 0; i < loop_count; i++)
			klib_putchar(' ');
		
		new_pos = cursor;
	}else{
		tty_putchar( CUR_ROW, CUR_COL, c);
		new_pos = cursor + 1;
	}

	klib_cursor_forward(new_pos);
}

void klib_print(char *str)
{
	if (!str || !*str){
		return;
	}

	while (*str){
		klib_putchar( *str++);
	}
}

void klib_putint(int num)
{
	char str[33] = {0};
	klib_print( klib_itoa(str, num));
}

void klib_info(char *info, int num, char* end)
{
    klib_print(info);
    klib_putint(num);
    klib_print(end);
}

void klib_clear()
{
	tty_clear();
	cursor = 0;
}

static void klib_cursor_forward(int new_pos)
{
	cursor = new_pos;
	while (cursor >= TTY_MAX_CHARS)
	{
		tty_roll_one_line();
		cursor -= TTY_MAX_COL;
	}
}

char *klib_itoa(char *str, int num)
{  
  char *p = str;  
  char ch;  
  int i;  
  int flag = 0;
  
  *p++ = '0';  
  *p++ = 'x';  
  if(num == 0)         
    *p++ = '0';  
  else  
  {    
    for(i=28;i>=0;i-=4)
    {  
      ch = (num >> i) & 0xF;  
      if(flag || (ch > 0))  
      {  
        flag = 1;  
        ch += '0';  
        if(ch > '9')   
          ch += 7;  
        *p++ = ch;  
      }  
    }  
  }  
  *p = 0;   
  return str;  
}

static unsigned int cur_block_top = KHEAP_BEGIN;
static kblock free_list[16] = {0}; // well, allocate more for reserve

static unsigned int kblk(unsigned page_count)
{
	unsigned ret = cur_block_top;

	if (page_count == 0)
	  return 0;

	if (cur_block_top + page_count * PAGE_SIZE >= KHEAP_END)
	  return 0;

	cur_block_top += page_count * PAGE_SIZE;
}

void* kmalloc(unsigned size)
{
	unsigned int ret = 0;
	int free_list_index = size / 8 + 1;
	kblock free_list_head = free_list[free_list_index];
	int index = 0;

	if (size > (4 * PAGE_SIZE)) // that's too large
		return NULL;
	
	if (free_list_head.next != NULL){
		kblock * block = free_list_head.next;
		free_list_head.next = free_list_head.next->next;
		block->next = 0;
		ret = (unsigned int)block;
		ret += sizeof(kblock);
		return (void*)ret;
	}

	for (index = 0; index < 16; index++){
	}

	return NULL;

}

void kfree(void* buf)
{
}

