
/*
 * Copyright (C) 2014  Ender Zheng
 * License: GPL version 2 or higher http://www.gnu.org/licenses/gpl.html
 */
#include "tty.h"
_START static void tty_copy_row(int src, int dst);
_START static void tty_clear_row(int row);
_STARTDATA static char *vidptr = (char*)0xb8000;

_START void tty_init(void)
{
	int i = 0;
	int j = 0;

	for (i = 0; i < TTY_MAX_ROW; i++){
		for (j = 0; j < TTY_MAX_COL; j++){
			tty_setcolor(i, j, clWhite, clBlack);
			tty_putchar(i, j, ' ');
		}
	}
}

_START void tty_setcolor(int x, int y, TTY_COLOR front, TTY_COLOR back)
{
	int color_field = (back << 4) | (front);
	int cur = ROW_COL_TO_CUR(x,y);

	if (x < 0 || x >= TTY_MAX_ROW ||
		y < 0 || y >= TTY_MAX_COL)
		return;

	vidptr[cur*2+1] = color_field;
}

_START TTY_COLOR tty_get_frontcolor(int x, int y)
{
	int cur = ROW_COL_TO_CUR(x,y);
	int color_field = 0;

	if (x < 0 || x >= TTY_MAX_ROW ||
		y < 0 || y >= TTY_MAX_COL)
		return clBlack;

	color_field = vidptr[cur*2+1];
	return (color_field % 8);
}

_START TTY_COLOR tty_get_backcolor(int x, int y)
{
	int cur = ROW_COL_TO_CUR(x,y);
	int color_field = 0;

	if (x < 0 || x >= TTY_MAX_ROW ||
		y < 0 || y >= TTY_MAX_COL)
		return clBlack;

	color_field = vidptr[cur*2+1];
	return (color_field >> 4);

}

_START void tty_putchar(int x, int y, char c)
{
	int cur = ROW_COL_TO_CUR(x,y);
	if (x < 0 || x >= TTY_MAX_ROW ||
		y < 0 || y >= TTY_MAX_COL)
		return;

	vidptr[cur*2] = c;
}

_START char tty_getchar(int x, int y)
{	
	int cur = ROW_COL_TO_CUR(x,y);
	if (x < 0 || x >= TTY_MAX_ROW ||
		y < 0 || y >= TTY_MAX_COL)
		return ' ';

	return vidptr[cur*2];
}

_START void tty_roll_one_line()
{
	int row = 0;
	for (row = 0; row < (TTY_MAX_ROW-1); row++)
		tty_copy_row(row+1, row);

	tty_clear_row(TTY_MAX_ROW-1);
}

_START static void tty_copy_row(int src, int dst)
{
	int col = 0;

	for (col = 0; col < TTY_MAX_COL; col++)
	  tty_putchar(dst, col, tty_getchar(src, col));
}

_START static void tty_clear_row(int row)
{
	int col = 0;

	for (col = 0; col < TTY_MAX_COL; col++)
		tty_putchar(row, col, ' ');
}

_START void tty_clear()
{
	int row = 0;

	for (row = 0; row < TTY_MAX_ROW; row++)
		tty_clear_row(row);
}
