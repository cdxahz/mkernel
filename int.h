
 #ifndef _INT_H_
 #define _INT_H_


#include "mm.h"

#define  INT_M_CTL     0x20  
#define  INT_M_CTLMASK 0x21  
#define  INT_S_CTL     0xA0  
#define  INT_S_CTLMASK 0xA1  
  

#define  INT_VECTOR_DIVIDE       0x0  
#define  INT_VECTOR_DEBUG        0x1  
#define  INT_VECTOR_NMI          0x2  
#define  INT_VECTOR_BREAKPOINT   0x3  
#define  INT_VECTOR_OVERFLOW     0x4  
#define  INT_VECTOR_BOUNDS       0x5    
#define  INT_VECTOR_INVAL_OP     0x6   
#define  INT_VECTOR_COPROC_NOT   0x7   
#define  INT_VECTOR_DOUBLE_FAULT 0x8   
#define  INT_VECTOR_COPROC_SEG   0x9   
#define  INT_VECTOR_INVAL_TSS    0xA   
#define  INT_VECTOR_SEG_NOT      0xB   
#define  INT_VECTOR_STACK_FAULT  0xC   
#define  INT_VECTOR_PROTECTION   0xD   
#define  INT_VECTOR_PAGE_FAULT   0xE   
 
#define  INT_VECTOR_COPROC_ERR   0x10  
 
 
#define  INT_VECTOR_IRQ0         0x20  
#define  INT_VECTOR_IRQ8         0x28  
 
  
 
#define  CLOCK_IRQ     0  
#define  KEYBOARD_IRQ  1  
  
 
#define  DA_386IGate       0x8E     
  
 
#define  KERNEL_PRIVILEGE  0  
#define  USER_PRIVILEGE    3  
  
#define  NULL_SELECTOR           0x0
#define  KERNEL_DATA_SELECTOR    0x8
#define  KERNEL_CODE_SELECTOR    0x10
#define  USER_DATA_SELECTOR      0x18
#define  USER_CODE_SELECTOR      0x20
#define  TSS_SELECTOR            0x28 // we are not going to use TSS when task switch, 
                                      //but we have to make tr register valid or x86 process
#define  SELECTOR_COUNT          6 // kernel 2 + user 2 + tss 1 + empty

#define  ADDRESS_LIMIT          0xfffff //  always 4k bytes algined, so last 0xfffff means 4G space

#define  SEG_CLASS_DATA         1
#define  SEG_CLASS_SYSTEM       0       // this is for TSS

#define  SEG_BASE_4K            1       // address count with 4k
#define  SEG_BASE_1             0       // address count with 1 byte

#define IDT_SIZE 256
  
  
 
typedef struct _segment_desc{
  unsigned long dword0 ;
  unsigned long dword1 ;
} segment_desc ;


  
 
 

extern unsigned char read_port(unsigned short port);
extern void write_port(unsigned short port, unsigned char data);
extern void asm_interrupt_handle_for_keyboard();
extern void shutdown();

_START void int_init();

_START void int_update_tss(void* address);

_START unsigned int_get_phymem_size();

void int_diags();


#endif
