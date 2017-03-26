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

_STARTDATA static unsigned long long idt[ IDT_SIZE ] ; 
_STARTDATA static unsigned long long idt_desc ;

unsigned long long *virtual_idt;
unsigned long long virtual_idt_desc;

_START static unsigned long long
make_gate (void (*function) (void), int dpl, int type)
{
  unsigned int e0, e1;


  e0 = (((unsigned int) function & 0xffff) /* Offset 15:0. */
        | (KERNEL_CODE_SELECTOR << 16)); /* Target code segment. */

  e1 = (((unsigned int) function & 0xffff0000) /* Offset 31:16. */
        | (1 << 15) /* Present. */
        | ((unsigned int) dpl << 13) /* Descriptor privilege level. */
        | (0 << 12) /* System. */
        | ((unsigned int) type << 8)); /* Gate type. */

  return e0 | ((unsigned long long) e1 << 32);
}

/* Creates an interrupt gate that invokes FUNCTION with the given
DPL. */
_START static unsigned long long
make_intr_gate (void (*function) (void), int dpl)
{
  return make_gate (function, dpl, 14);
}

/* Creates a trap gate that invokes FUNCTION with the given
DPL. */
_START static unsigned long long
make_trap_gate (void (*function) (void), int dpl)
{
  return make_gate (function, dpl, 15);
}

/* Returns a descriptor that yields the given LIMIT and BASE when
used as an operand for the LIDT instruction. */
_START static  unsigned long long
make_idtr_operand (unsigned short limit, void *base)
{
  return limit | ((unsigned long long) (unsigned int) base << 16);
}


extern _STARTDATA unsigned long intr_stubs[IDT_SIZE];
unsigned long *virtual_intr_stubs;

_START static void setup_idt()
{
	int i = 0;
	for (i = 0; i < IDT_SIZE; i++){
		idt[i] = make_intr_gate (intr_stubs[i], 0);
	}
}

/* Sends an end-of-interrupt signal to the PIC for the given IRQ.
If we don't acknowledge the IRQ, it will never be delivered to
us again, so this is important. */
static void
pic_end_of_interrupt (int irq)
{

  /* Acknowledge master PIC. */
  write_port (0x20, 0x20);

  /* Acknowledge slave PIC if this is a slave interrupt. */
  if (irq >= 0x28)
    write_port (0xa0, 0x20);
}

static char* intr_names[IDT_SIZE];
static int_callback in_callbacks[IDT_SIZE];

void int_register(int vec_no, int_callback fn, int is_trap, int dpl)
{
	if (vec_no < 0 || vec_no >= IDT_SIZE)
	  return;
	
	if (is_trap){
		virtual_idt[vec_no] = make_trap_gate(virtual_intr_stubs[vec_no], dpl);
	}else{
		virtual_idt[vec_no] = make_intr_gate(virtual_intr_stubs[vec_no], dpl);
	}

	in_callbacks[vec_no] = fn;
}

void int_unregister(int vec_no)
{
    if (vec_no < 0 || vec_no >= IDT_SIZE){
		printf("fatal error: vec number %x invalid!!\n", vec_no);
		return;
	}

    in_callbacks[vec_no] = 0;
    virtual_idt[vec_no] = 0;
}

void
intr_handler (intr_frame *frame)
{
	// printf("interrupt: [%x] %s\n", frame->vec_no, intr_names[frame->vec_no]);
	int external = frame->vec_no >= 0x20 && frame->vec_no < 0x30;
	int_callback fn = 0;
	
	if (frame->vec_no < 0 || frame->vec_no >= IDT_SIZE){
		printf("fatal error: vec number %x invalid!!\n", frame->vec_no);
		return;
	}

	fn = in_callbacks[frame->vec_no];

	if(fn)
	  fn(frame);
	else{
	   printf("int %x: %s", frame->vec_no, intr_names[frame->vec_no]);
    }

	if (external)
		pic_end_of_interrupt(frame->vec_no);
}



void int_enable_all()
{
	int i = 0;
	virtual_intr_stubs = (unsigned int)intr_stubs + KERNEL_OFFSET;
	virtual_idt = (unsigned int)idt + KERNEL_OFFSET;
	virtual_idt_desc = make_idtr_operand (sizeof idt - 1, virtual_idt);
	__asm__ volatile ("lidt %0\nsti" : : "m" (virtual_idt_desc));

  write_port( 0x21 , 0x0 ) ;
  write_port( 0xA1 , 0x0 ) ;
	
	/* Initialize intr_names. */
  for (i = 0; i < IDT_SIZE; i++){
	intr_names[i] = "unknown";
	in_callbacks[i] = 0;
  }
  intr_names[0] = "#DE Divide Error";
  intr_names[1] = "#DB Debug Exception";
  intr_names[2] = "NMI Interrupt";
  intr_names[3] = "#BP Breakpoint Exception";
  intr_names[4] = "#OF Overflow Exception";
  intr_names[5] = "#BR BOUND Range Exceeded Exception";
  intr_names[6] = "#UD Invalid Opcode Exception";
  intr_names[7] = "#NM Device Not Available Exception";
  intr_names[8] = "#DF Double Fault Exception";
  intr_names[9] = "Coprocessor Segment Overrun";
  intr_names[10] = "#TS Invalid TSS Exception";
  intr_names[11] = "#NP Segment Not Present";
  intr_names[12] = "#SS Stack Fault Exception";
  intr_names[13] = "#GP General Protection Exception";
  intr_names[14] = "#PF Page-Fault Exception";
  intr_names[16] = "#MF x87 FPU Floating-Point Error";
  intr_names[17] = "#AC Alignment Check Exception";
  intr_names[18] = "#MC Machine-Check Exception";
  intr_names[19] = "#XF SIMD Floating-Point Exception";
  intr_names[32] = "timer";
  intr_names[33] = "keyboard";

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


_STARTDATA static unsigned long long gdt[SELECTOR_COUNT];
_STARTDATA unsigned long long gdtr_operand;

static unsigned long long * virtual_gdt = (gdt + KERNEL_OFFSET);

_START static void setup_gdt()
{

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
   gdtr_operand = make_gdtr_operand (sizeof gdt - 1, gdt);
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
    gdt[TSS_SELECTOR / 8] =          make_seg_desc(base, 0x67,       SEG_CLASS_SYSTEM,   9,  KERNEL_PRIVILEGE, SEG_BASE_1);
    RELOAD_TSS(TSS_SELECTOR);
}

_START void int_init()
{

    init_interrupt();

    setup_gdt();


    setup_idt();

	//int_diags();
}


void int_diags()
{
	int _cr0 = 0;
    int _cr3 = 0;
	int a20 = 0;
    int cs = 0 ;
    int ds = 0;
    int esp = 0;
    int eip = 0;
    unsigned long addr;
    unsigned mem;
	// output cr0
	__asm__( "movl %%cr0, %0" : "=r"(_cr0) );
    printf("cr0: \t%x\n", _cr0);
	printf("---------------\n");

	printf("\t\tprotected mode enabled: %d\n", (_cr0 & 0x1) != 0);
	printf("\t\tpaging enabled: %d\n", (_cr0 & 0x80000000) != 0  );


	__asm__( "movl %%cr3, %0" : "=r"(_cr3));
    printf("cr3: \t%x\n", _cr3);

    __asm__( "movl %%cs, %0" : "=r"(cs));
    printf("cs: \t%x\n", cs);

    __asm__( "movl %%fs, %0" : "=r"(ds));
    printf("ds: \t%x\n", ds);

    __asm__( "movl %%esp, %0" : "=r"(esp));
    printf("esp: \t%x\n", esp);


	// output A20
	a20 = read_port(0x92);
	printf("a20: \t%x\n", a20);


    // start function address

}

#define GET_INTR_FLAG(flag)\
    __asm__("pushfl"); \
    __asm__("popl %0" : "=q"(flag));

unsigned int_is_intr_enabled()
{
    unsigned int flags;

    GET_INTR_FLAG(flags);

    return (flags & 0x00000200) == 0x00000200 ;
}

unsigned int_intr_enable()
{
    unsigned old = int_is_intr_enabled();

    __asm__("sti");

    return old;
}

unsigned int_intr_disable()
{
    unsigned old = int_is_intr_enabled();

    __asm__("cli" : : : "memory");

    return old;
}

