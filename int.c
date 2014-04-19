#include "int.h"
#include "keyboard.h"


void int_init()
{
  segment_desc idt[ IDT_SIZE ] ; 
  unsigned long idt_desc[ 2 ] ;
  unsigned long idt_addr ;
  unsigned long keyboard_addr ;
  unsigned long idt_offset = 0x8 ; 

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

  // setup keyboard
  keyboard_addr = ( unsigned long )asm_interrupt_handle_for_keyboard ; 
  idt[ 0x21 ].dword0 = ( keyboard_addr & 0xffff ) | ( idt_offset << 16 ) ;
  idt[ 0x21 ].dword1 = ( keyboard_addr & 0xffff0000 ) | 0x8e00 ;


  idt_addr = ( unsigned long )idt ;
  idt_desc[ 0 ] = 0x800 + ( ( idt_addr & 0xffff ) << 16 ) ;
  idt_desc[ 1 ] = idt_addr >> 16 ;

  __asm__( "lidt %0\n""sti" : "=m"( idt_desc ) ) ; 

	int_diags();
}

void int_diags()
{
	int _cr0;
	int a20;
	// output cr0
	__asm__( "movl %%cr0, %0" : "=r"(_cr0) );
	klib_print("cr0: ");
	klib_putint(_cr0);
	klib_print("---------------\n");

	klib_print("\t protected mode enabled: ");
	klib_putint( _cr0 & 0x1);
	klib_print("\t paging enabled: ");
	klib_putint( _cr0 & 0x80000000);
	
	klib_print("---------------\n");
	// output A20
	a20 = read_port(0x92);
	klib_print("a20: ");
	klib_putint(a20);
}

