
 #ifndef _INT_H_
 #define _INT_H_

#include "config.h"
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
  
 
 
/* Interrupt stack frame. */
typedef struct _intr_frame
  {
    /* Pushed by intr_entry in intr-stubs.S.
These are the interrupted task's saved registers. */
    unsigned int edi; /* Saved EDI. */
    unsigned int esi; /* Saved ESI. */
    unsigned int ebp; /* Saved EBP. */
    unsigned int esp_dummy; /* Not used. */
    unsigned int ebx; /* Saved EBX. */
    unsigned int edx; /* Saved EDX. */
    unsigned int ecx; /* Saved ECX. */
    unsigned int eax; /* Saved EAX. */
    unsigned short gs, :16; /* Saved GS segment register. */
    unsigned short fs, :16; /* Saved FS segment register. */
    unsigned short es, :16; /* Saved ES segment register. */
    unsigned short ds, :16; /* Saved DS segment register. */

    /* Pushed by intrNN_stub in intr-stubs.S. */
    unsigned int vec_no; /* Interrupt vector number. */

    /* Sometimes pushed by the CPU,
otherwise for consistency pushed as 0 by intrNN_stub.
The CPU puts it just under `eip', but we move it here. */
    unsigned int error_code; /* Error code. */

    /* Pushed by intrNN_stub in intr-stubs.S.
This frame pointer eases interpretation of backtraces. */
    void *frame_pointer; /* Saved EBP (frame pointer). */

    /* Pushed by the CPU.
These are the interrupted task's saved registers. */
    void (*eip) (void); /* Next instruction to execute. */
    unsigned short cs, :16; /* Code segment for eip. */
    unsigned int eflags; /* Saved CPU flags. */
    void *esp; /* Saved stack pointer. */
    unsigned short ss, :16; /* Data segment for esp. */
  }intr_frame;

  
 
 

extern unsigned char read_port(unsigned short port);
extern void write_port(unsigned short port, unsigned char data);
extern void asm_interrupt_handle_for_keyboard();
extern void shutdown();

_START void int_init();

_START void int_update_tss(void* address);

_START unsigned int_get_phymem_size();

void int_diags();

void int_enable_all();

typedef void (*int_callback)(intr_frame* frame);

void int_register(int vec_no, int_callback fn, int is_trap, int dpl);
#endif
