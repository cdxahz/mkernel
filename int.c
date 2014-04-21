#include "int.h"
#include "keyboard.h"


_START static void init_interrupt()
{


  write_port( 0x20 , 0x11 ) ;
  write_port( 0xA0 , 0x11 ) ;
  write_port( 0x21 , 0x20 ) ;
  write_port( 0xA1 , 0x28 ) ;

  write_port( 0x21 , 0x4 ) ;  
  write_port( 0xA1 , 0x2 ) ;  

  write_port( 0x21 , 0x1 ) ;
  write_port( 0xA1 , 0x1 ) ;

  write_port( 0x21 , 0xFF ) ;
  write_port( 0xA1 , 0xFF ) ;
}

static segment_desc virtual_idt[ IDT_SIZE ] ; 
static unsigned long virtual_idt_desc[ 2 ] ;

_START static void setup_idt()
{
    unsigned long keyboard_addr ;
    segment_desc *idt = (virtual_idt - KERNEL_OFFSET);
    unsigned long *idt_desc = (virtual_idt_desc - KERNEL_OFFSET);
    unsigned long idt_addr ;

    unsigned long code_selector = KERNEL_CODE_SELECTOR ; 
    // setup keyboard

    keyboard_addr = ( unsigned long )asm_interrupt_handle_for_keyboard ; 
    keyboard_addr -= KERNEL_OFFSET;
    idt[ 0x21 ].dword0 = ( keyboard_addr & 0xffff ) | ( code_selector << 16 ) ;
    idt[ 0x21 ].dword1 = ( keyboard_addr & 0xffff0000 ) | 0x8e00 ;


    idt_addr = ( unsigned long )idt ;
    idt_desc[ 0 ] = 0x800 + ( ( idt_addr & 0xffff ) << 16 ) ;
    idt_desc[ 1 ] = idt_addr >> 16 ;

    __asm__( "lidt %0\n""sti" : "=m"( idt_desc ) ) ; 

}

/* Returns a segment descriptor with the given 32-bit BASE and
20-bit LIMIT (whose interpretation depends on GRANULARITY).
The descriptor represents a system or code/data segment
according to CLASS, and TYPE is its type (whose interpretation
depends on the class).

The segment has descriptor privilege level DPL, meaning that
it can be used in rings numbered DPL or lower. In practice,
DPL==3 means that user processes can use the segment and
DPL==0 means that only the kernel can use the segment. See
[IA32-v3a] 4.5 "Privilege Levels" for further discussion. */
_START static unsigned long long
make_seg_desc (unsigned int base,
               unsigned int limit,
               unsigned int class,
               int type,
               int dpl,
               unsigned int granularity)
{
  unsigned e0, e1;


  e0 = ((limit & 0xffff) /* Limit 15:0. */
        | (base << 16)); /* Base 15:0. */

  e1 = (((base >> 16) & 0xff) /* Base 23:16. */
        | (type << 8) /* Segment type. */
        | (class << 12) /* 0=system, 1=code/data. */
        | (dpl << 13) /* Descriptor privilege. */
        | (1 << 15) /* Present. */
        | (limit & 0xf0000) /* Limit 16:19. */
        | (1 << 22) /* 32-bit segment. */
        | (granularity << 23) /* Byte/page granularity. */
        | (base & 0xff000000)); /* Base 31:24. */

  return e0 | ((unsigned long long) e1 << 32);
}

_START static unsigned long long
make_gdtr_operand (unsigned short limit, void *base)
{
  return limit | ((unsigned long long) (unsigned int) base << 16);
}

#define RELOAD_CS(SELECTOR) \
    __asm__ ("ljmp %0, $L\nL:nop" : : "I"(SELECTOR));

#define RELOAD_DS(SELECTOR) \
    __asm__ ("movw %0, %%ax" : : "I"(SELECTOR)); \
    __asm__ ("movw %ax, %ds");\
    __asm__ ("movw %ax, %es");\
    __asm__ ("movw %ax, %fs");\
    __asm__ ("movw %ax, %gs");\
    __asm__ ("movw %ax, %ss");

#define RELOAD_TSS(SELECTOR) \
    __asm__  ("ltr %w0" : : "q" (SELECTOR));

#define RELOAD_GDT(gdtr) \
    __asm__  ("lgdt %0" : : "m" (gdtr));


static unsigned long long virtual_gdt[SELECTOR_COUNT];

_START static void setup_gdt()
{
   unsigned long long gdtr_operand;
   unsigned long long *gdt = (&virtual_gdt - KERNEL_OFFSET);

   gdt[0] = 0;
   gdt[KERNEL_CODE_SELECTOR / 8] =  make_seg_desc(0, ADDRESS_LIMIT, SEG_CLASS_DATA,     10, KERNEL_PRIVILEGE, SEG_BASE_4K); 
   gdt[KERNEL_DATA_SELECTOR / 8] =  make_seg_desc(0, ADDRESS_LIMIT, SEG_CLASS_DATA,     2,  KERNEL_PRIVILEGE, SEG_BASE_4K);
   gdt[USER_CODE_SELECTOR / 8] =    make_seg_desc(0, ADDRESS_LIMIT, SEG_CLASS_DATA,     10, USER_PRIVILEGE, SEG_BASE_4K); 
   gdt[USER_DATA_SELECTOR / 8] =    make_seg_desc(0, ADDRESS_LIMIT, SEG_CLASS_DATA,     2,  USER_PRIVILEGE, SEG_BASE_4K); 
   gdt[TSS_SELECTOR / 8] =          make_seg_desc(0, 0x67,          SEG_CLASS_SYSTEM,   9,  KERNEL_PRIVILEGE, SEG_BASE_1); // tmp

   /*
    * because all of the kernel&user selector use the same base & limit 
    * actually any code & data see the same address space , only with 
    * class (readonly/executable) and privilege different 
    * for tss processor read physical memory from tss.base directly 
    */
   gdtr_operand = make_gdtr_operand (sizeof virtual_gdt - 1, gdt);
   RELOAD_GDT(gdtr_operand);
   RELOAD_CS(KERNEL_CODE_SELECTOR);
   RELOAD_DS(KERNEL_DATA_SELECTOR);
}

/*
 * address should be a physical address
 */
_START void int_update_tss(void* address)
{
    unsigned int base = (unsigned int)address;
    unsigned long long *gdt = (&virtual_gdt - KERNEL_OFFSET);
    gdt[TSS_SELECTOR / 8] =          make_seg_desc(base, 0x67,       SEG_CLASS_SYSTEM,   9,  KERNEL_PRIVILEGE, SEG_BASE_1);
    RELOAD_TSS(TSS_SELECTOR);
}

_START void int_init()
{

    init_interrupt();

    // setup_gdt();

    setup_idt();

	// int_diags();
}



_START void int_diags()
{
	int _cr0;
    int _cr3;
	int a20;
    int cs;
    int ds;
    int esp;
    int eip;
    unsigned long addr;
    unsigned mem;
	// output cr0
	__asm__( "movl %%cr0, %0" : "=r"(_cr0) );
	klib_print("cr0: ");
	klib_putint(_cr0);
	klib_print("---------------\n");

	klib_print("\t protected mode enabled: ");
	klib_putint( _cr0 & 0x1);
	klib_print("\t paging enabled: ");
	klib_putint( _cr0 & 0x80000000);
    klib_print("\n");

	__asm__( "movl %%cr3, %0" : "=r"(_cr3));
    klib_print("cr3: ");
    klib_putint(_cr3);

    __asm__( "movl %%cs, %0" : "=r"(cs));
    klib_print("cs: ");
    klib_putint(cs);

    __asm__( "movl %%fs, %0" : "=r"(ds));
    klib_print("ds: ");
    klib_putint(ds);

    __asm__( "movl %%esp, %0" : "=r"(esp));
    klib_print("esp: ");
    klib_putint(esp);


	// output A20
	a20 = read_port(0x92);
	klib_print("a20: ");
	klib_putint(a20);


    // start function address

}

