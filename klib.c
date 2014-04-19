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


void klib_putchar(char c)
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
	if (!str || !*str)
	  return;

	while (*str){
		klib_putchar( *str++);
	}
}

void klib_putint(int num)
{
	char str[33] = {0};
	klib_print( klib_itoa(str, num));
	klib_putchar('\n');
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
