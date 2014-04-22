bits 32

global write_port
global read_port
global asm_interrupt_handle_for_keyboard
global shutdown
extern kb_process


section .start
write_port:
  mov   edx, [esp + 4]    
  mov   al, [esp + 4 + 4]  
  out   dx, al  
  nop   
  nop  
  ret  

section .start
read_port:
  mov   edx, [esp + 4] 
  xor   eax, eax  
  in    al, dx  
  nop   
  nop  
  ret  


section .start
shutdown:
          mov ax,5300h
          mov bx,0
          int 15h
          push ax

          mov ax,5308h
          mov bx,1
          mov cx,1
          int 15h
          mov ax,5308h
          mov bx,0ffffh
          mov cx,1
          int 15h

          mov ax,5301h
          mov bx,0
          int 15h
          mov ax,530Eh
          mov bx,0
          pop cx

          int 15h
          mov ax,530Dh
          mov bx,1
          mov cx,1
          int 15h
          mov ax,530Fh
          mov bx,1
          mov cx,1
          int 15h
          mov ax,5307h
          mov bx,1
          mov cx,3
          int 15h
          int 20h
 
section .start
asm_interrupt_handle_for_keyboard:
  pushad      
  push    ds  
  push    es  
  push    fs  
  push    gs  
  
  in      al, 0x21        
  or      al, (1 << 1)             
  out     0x21, al        
  mov     al, 0x20                  
  out     0x20, al            
  sti      
  call    kb_process               
  cli  
  in      al, 0x21        
  and     al, ~(1 << 1)            
  out     0x21, al        
  
  pop     gs  
  pop     fs  
  pop     es  
  pop     ds  
  popad       
  iretd  

