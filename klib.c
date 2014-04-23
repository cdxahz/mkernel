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

// following for malloc/free
static unsigned int cur_block_top = KHEAP_BEGIN;
static kblock free_list[513] = { 0 }; 
static unsigned int kblk(unsigned page_count);
static void klib_add_to_free_list(int index, kblock* buf);




void* kmalloc(unsigned size)
{
	unsigned int ret = 0;
	int free_list_index = size / 8 + 1;
	kblock* free_list_head = &free_list[free_list_index];
	int index = 0;
	int sliced = 0;

	if (size > (PAGE_SIZE)) // that's too large
		return NULL;

	if (size == 0)
		return NULL;

	if (free_list_head->next == NULL){
		for (index = free_list_index + 1; index <= 512; index++){
			int borrow_size, left_count;
			kblock* addr;
			int left_index;
			if (free_list[index].next == 0)
				continue;

			borrow_size = free_list_index;
			left_count = index - borrow_size;

			if (left_count < 2)
				continue;

			addr = (kblock*)free_list[index].next;
			free_list[index].next = addr->next;
			addr->next = 0;

			klib_add_to_free_list(free_list_index, (void*)addr);
			left_index = left_count - 1;
			klib_add_to_free_list(left_index, (void*)((unsigned int)addr + 8 * (borrow_size + 1)));

			break;
		}

		if (index == 513){
			int left_index;
			int borrow_size = free_list_index;
			unsigned int vir = kblk(1);
			klib_add_to_free_list(free_list_index, (void*)vir);
			left_index = (PAGE_SIZE / 8) - free_list_index - 1;
			klib_add_to_free_list(left_index, (void*)((unsigned int)vir + 8 * (borrow_size + 1)));
		}
	}

	if (free_list_head->next != NULL){
		kblock * block = free_list_head->next;
		free_list_head->next = free_list_head->next->next;
		block->next = 0;
		block->size = size;
		ret = (unsigned int)block;
		ret += sizeof(kblock);
		return (void*)ret;
	}



	return NULL;

}

void kfree(void* buf)
{
	kblock* block = NULL;
	int size = 0;
	int free_list_index;
	if (buf == NULL)
		return;

	block = (kblock*)((unsigned int)buf - 8);
	size = block->size;
	free_list_index = size / 8 + 1;

	klib_add_to_free_list(free_list_index, block);
}


static unsigned int kblk(unsigned page_count)
{
	unsigned ret = cur_block_top;

	if (page_count == 0)
		return 0;

	if (cur_block_top + page_count * PAGE_SIZE >= KHEAP_END)
		return 0;

	cur_block_top += page_count * PAGE_SIZE;
	//printf("BUTTON %p, TOP %p, current %p, used %d(pages)\n", KHEAP_BEGIN, KHEAP_END, cur_block_top,
	//	(cur_block_top - KHEAP_BEGIN) / (PAGE_SIZE));

	return ret;
}


static void klib_add_to_free_list(int index, kblock* buf)
{
	kblock *head = NULL;
	kblock *node = NULL;
	kblock *pre = NULL;
	kblock *ppre = NULL;

	unsigned int pre_addr = (unsigned int)pre;
	unsigned int node_addr = (unsigned int)node;
	unsigned int buf_addr = (unsigned int)buf;

	int merged = 0;
	int merged_index = 0;

	if (index <= 0 || index > 512)
		return;


	head = &free_list[index];

	node = head->next;
	pre = head;
	ppre = NULL;
	while (node){
		unsigned int addr = (unsigned int)node;
		unsigned int inserted = (unsigned int)buf;
		if (addr > inserted){
			break;
		}
		ppre = pre;
		pre = node;
		node = node->next;
	}

	pre_addr = (unsigned int)pre;
	node_addr = (unsigned int)node;
	buf_addr = (unsigned int)buf;

	if (pre != head){
		merged_index = index * 2 + 1;
		if ((pre_addr + 8 * (index + 1) == buf_addr) && (merged_index <= 512)){
			// merge pre
			ppre->next = node;
			buf->next = NULL;
			pre->next = NULL;
			klib_add_to_free_list(merged_index, pre);
			merged = 1;
			//printf("[pre+buf]two %d blocks merged into %d block\n", index, merged_index);
		}
	} 

	if (!merged && node != NULL){
		merged_index = index * 2 + 1;
		if ((buf_addr + 8 * (index + 1) == node_addr) && (merged_index <= 512)){
			// merge node
			pre->next = node->next;
			buf->next = NULL;
			node->next = NULL;
			klib_add_to_free_list(merged_index, buf);
			merged = 1;
			//printf("[buf+node]two %d blocks merged into %d block\n", index, merged_index);
		}
	}

	if (!merged){
		pre->next = (kblock*)buf;
		((kblock*)buf)->next = node;
	}

}

void memcpy(void* _src, void* _dst, unsigned len)
{
	char* src = _src;
	char* dst = _dst;

	int i = 0;
    for (i = 0; i < len; i++) {
        src[i] = dst[i];
    }
}

void memmove(void* _src, void* _dst, unsigned len)
{
	char* src = _src;
	char* dst = _dst;

	int i = 0;

    if (src >= dst) {
		for (i = 0; i < len; i++) {
			src[i] = dst[i];
		}
    }else{
		for (i = len - 1; i >= 0; i--) {
			src[i] = dst[i];
		}
	}
    
}

int memcmp(void* _src, void* _dst, unsigned len)
{
	char* src = _src;
	char* dst = _dst;
	int i = 0;

    for (i = 0; i < len; i++) {
        if (src[i] > dst[i]) {
            return 1;
        }else if(src[i] < dst[i]){
			return -1;
		}
    }

	return 0;
}

void memset(void* _src, char val, int len)
{
	char* src = _src;
	int i = 0;

    for (i = 0; i < len; i++) {
        src[i] = val;
    }
}
 
unsigned strlen(const char* str)
{
	int count = 0;
    while (*str++) {
        count++;
    }
	return count;
}

char* strcpy(char* dst, const char* src)
{
    while (*dst++ = *src++);

    return dst;
}

char* strstr(const char* src, const char* str)
{
	// FIXME
	return NULL;
}

char* strrev(char *src)
{
	int len = strlen(src);
	int mid = len / 2;
	int i = 0;

    for (i = 0; i < mid; i++) {
        char tmp = src[i];
		src[i] = src[len - i -1];
		src[len - i - 1] = tmp;
    }
}

int strcmp(char* src, char* dst)
{
	int src_len = strlen(src);
	int dst_len = strlen(dst);
	int len = src_len > dst_len ? dst_len : src_len;
	int i = 0;

    for (i = 0; i < len; i++) {
        if (src[i] > dst[i]) {
            return 1;
        }else if(src[i] < dst[i]){
			return -1;
		}
    }

    if (src_len > len) {
        return 1;
    }else if(dst_len > len){
		return -1;
	}else{
		return 0;
	}
}

void printf(const char* str, ...)
{
	va_list ap;
	va_start(ap, str);
	vprintf(str,ap);
	va_end(ap);
}

void vprintf(const char* src, va_list ap)
{
	int len = strlen(src);
	int i = 0;

    for (i = 0; i < len; i++) {
        char cur = src[i];
        if (cur == '%') {
			cur = src[i+1];
            switch (cur) {
			case '%':
				klib_putchar(src[i+1]);
				break;
			case 'd':
				{
					int arg = va_arg(ap, int);
					char* s = itoa(arg, 10, 1);
					klib_print(s);
					kfree(s);
					break;
				}
			case 'x':
			case 'p':
				{
					int arg = va_arg(ap, int);
					char* s = itoa(arg, 16, 0);
					klib_print(s);
					kfree(s);
					break;
				}
			case 'u':
				{
					int arg = va_arg(ap, int);
					char* s = itoa(arg, 10, 0);
					klib_print(s);
					kfree(s);
					break;
				}
			case 's':
				{
					char* arg = va_arg(ap, char*);
					klib_print(arg);
					break;
				}
			default:
				{
					klib_putchar('?');
				}
            }
			i++;
        }else{
			klib_putchar(cur);
		}
    }
}

static char _num(int i){

    if (i < 0) {
        i = 0 - i;
    }
    if (i >= 0 && i < 10) {
        return i + '0';
    }

	return (i - 10) + 'a';
}

char* itoa(int num, int base, int sign)
{
	int str_len = 12;
	char* str = kmalloc(12);
	char* ret = str;
	int left = num;
    unsigned uleft = num;
	int cur = 0;
    unsigned ucur = 0;
	char *begin;

	memset(str, 0, 12);

    if (base != 10 && base != 16) {
		kfree(str);
        return NULL;
    }


	if ((base == 10) && sign && left < 0) {
		str[0] = '-';
		str++;
		left = (0 - left);
	}else if(base == 16){
		str[0] = '0';
		str[1] = 'x';
		str += 2;
	}

	begin = str;
    if (sign && (base != 16)) {
        while (left) {
            cur = left % base;
            *str = _num(cur);
            str++;
            left = left / base;
        }
    }else{
        while (uleft) {
            ucur = uleft % base;
            *str = _num(ucur);
            str++;
            uleft = uleft / base;
        }
    }

	strrev(begin);

	return ret;
}


