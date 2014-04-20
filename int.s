	.file	"int.c"
	.text
	.type	init_interrupt, @function
init_interrupt:
.LFB0:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$24, %esp
	movl	$17, 4(%esp)
	movl	$32, (%esp)
	call	write_port
	movl	$17, 4(%esp)
	movl	$160, (%esp)
	call	write_port
	movl	$32, 4(%esp)
	movl	$33, (%esp)
	call	write_port
	movl	$40, 4(%esp)
	movl	$161, (%esp)
	call	write_port
	movl	$4, 4(%esp)
	movl	$33, (%esp)
	call	write_port
	movl	$2, 4(%esp)
	movl	$161, (%esp)
	call	write_port
	movl	$1, 4(%esp)
	movl	$33, (%esp)
	call	write_port
	movl	$1, 4(%esp)
	movl	$161, (%esp)
	call	write_port
	movl	$255, 4(%esp)
	movl	$33, (%esp)
	call	write_port
	movl	$255, 4(%esp)
	movl	$161, (%esp)
	call	write_port
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE0:
	.size	init_interrupt, .-init_interrupt
	.type	setup_idt, @function
setup_idt:
.LFB1:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$40, %esp
	movl	$8, -12(%ebp)
	movl	$asm_interrupt_handle_for_keyboard, -16(%ebp)
	movl	-16(%ebp), %eax
	movl	%eax, (%esp)
	call	klib_putint
	movl	-16(%ebp), %eax
	movzwl	%ax, %edx
	movl	-12(%ebp), %eax
	sall	$16, %eax
	orl	%edx, %eax
	movl	%eax, idt.1374+264
	movl	-16(%ebp), %eax
	andl	$-65536, %eax
	orb	$142, %ah
	movl	%eax, idt.1374+268
	movl	$idt.1374, -20(%ebp)
	movl	-20(%ebp), %eax
	sall	$16, %eax
	addl	$2048, %eax
	movl	%eax, idt_desc.1375
	movl	-20(%ebp), %eax
	shrl	$16, %eax
	movl	%eax, idt_desc.1375+4
/APP
/  43 "int.c" 1
	lidt idt_desc.1375
sti
/  0 "" 2
/NO_APP
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE1:
	.size	setup_idt, .-setup_idt
	.type	make_seg_desc, @function
make_seg_desc:
.LFB2:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$20, %esp
	.cfi_offset 7, -12
	.cfi_offset 6, -16
	.cfi_offset 3, -20
	movl	12(%ebp), %eax
	movzwl	%ax, %edx
	movl	8(%ebp), %eax
	sall	$16, %eax
	orl	%edx, %eax
	movl	%eax, -20(%ebp)
	movl	8(%ebp), %eax
	shrl	$16, %eax
	movzbl	%al, %edx
	movl	20(%ebp), %eax
	sall	$8, %eax
	orl	%eax, %edx
	movl	16(%ebp), %eax
	sall	$12, %eax
	orl	%eax, %edx
	movl	24(%ebp), %eax
	sall	$13, %eax
	orl	%eax, %edx
	movl	12(%ebp), %eax
	andl	$983040, %eax
	orl	%eax, %edx
	movl	28(%ebp), %eax
	sall	$23, %eax
	orl	%eax, %edx
	movl	8(%ebp), %eax
	andl	$-16777216, %eax
	orl	%edx, %eax
	orl	$4227072, %eax
	movl	%eax, -24(%ebp)
	movl	-20(%ebp), %ecx
	movl	$0, %ebx
	movl	-24(%ebp), %eax
	movl	$0, %edx
	movl	%eax, %edx
	movl	$0, %eax
	movl	%ecx, %esi
	orl	%eax, %esi
	movl	%ebx, %edi
	orl	%edx, %edi
	movl	%esi, %eax
	movl	%edi, %edx
	addl	$20, %esp
	popl	%ebx
	.cfi_restore 3
	popl	%esi
	.cfi_restore 6
	popl	%edi
	.cfi_restore 7
	popl	%ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE2:
	.size	make_seg_desc, .-make_seg_desc
	.type	make_gdtr_operand, @function
make_gdtr_operand:
.LFB3:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$12, %esp
	.cfi_offset 7, -12
	.cfi_offset 6, -16
	.cfi_offset 3, -20
	movl	8(%ebp), %eax
	movw	%ax, -20(%ebp)
	movzwl	-20(%ebp), %eax
	movl	$0, %edx
	movl	12(%ebp), %ecx
	movl	$0, %ebx
	shldl	$16, %ecx, %ebx
	sall	$16, %ecx
	movl	%eax, %esi
	orl	%ecx, %esi
	movl	%edx, %edi
	orl	%ebx, %edi
	movl	%esi, %eax
	movl	%edi, %edx
	addl	$12, %esp
	popl	%ebx
	.cfi_restore 3
	popl	%esi
	.cfi_restore 6
	popl	%edi
	.cfi_restore 7
	popl	%ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE3:
	.size	make_gdtr_operand, .-make_gdtr_operand
	.local	gdt
	.comm	gdt,48,32
	.type	setup_gdt, @function
setup_gdt:
.LFB4:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$40, %esp
	movl	$0, gdt
	movl	$0, gdt+4
	movl	$1, 20(%esp)
	movl	$0, 16(%esp)
	movl	$10, 12(%esp)
	movl	$1, 8(%esp)
	movl	$1048575, 4(%esp)
	movl	$0, (%esp)
	call	make_seg_desc
	movl	%eax, gdt+8
	movl	%edx, gdt+12
	movl	$1, 20(%esp)
	movl	$0, 16(%esp)
	movl	$2, 12(%esp)
	movl	$1, 8(%esp)
	movl	$1048575, 4(%esp)
	movl	$0, (%esp)
	call	make_seg_desc
	movl	%eax, gdt+16
	movl	%edx, gdt+20
	movl	$1, 20(%esp)
	movl	$3, 16(%esp)
	movl	$10, 12(%esp)
	movl	$1, 8(%esp)
	movl	$1048575, 4(%esp)
	movl	$0, (%esp)
	call	make_seg_desc
	movl	%eax, gdt+32
	movl	%edx, gdt+36
	movl	$1, 20(%esp)
	movl	$3, 16(%esp)
	movl	$2, 12(%esp)
	movl	$1, 8(%esp)
	movl	$1048575, 4(%esp)
	movl	$0, (%esp)
	call	make_seg_desc
	movl	%eax, gdt+24
	movl	%edx, gdt+28
	movl	$0, 20(%esp)
	movl	$0, 16(%esp)
	movl	$9, 12(%esp)
	movl	$0, 8(%esp)
	movl	$103, 4(%esp)
	movl	$0, (%esp)
	call	make_seg_desc
	movl	%eax, gdt+40
	movl	%edx, gdt+44
	movl	$gdt, 4(%esp)
	movl	$47, (%esp)
	call	make_gdtr_operand
	movl	%eax, -8(%ebp)
	movl	%edx, -4(%ebp)
/APP
/  114 "int.c" 1
	lgdt -8(%ebp)
/  0 "" 2
/NO_APP
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE4:
	.size	setup_gdt, .-setup_gdt
	.globl	int_update_tss
	.type	int_update_tss, @function
int_update_tss:
.LFB5:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$40, %esp
	movl	8(%ebp), %eax
	movl	%eax, -4(%ebp)
	movl	$0, 20(%esp)
	movl	$0, 16(%esp)
	movl	$9, 12(%esp)
	movl	$0, 8(%esp)
	movl	$103, 4(%esp)
	movl	-4(%ebp), %eax
	movl	%eax, (%esp)
	call	make_seg_desc
	movl	%eax, gdt+40
	movl	%edx, gdt+44
	movl	$40, %eax
/APP
/  125 "int.c" 1
	ltr %ax
/  0 "" 2
/NO_APP
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE5:
	.size	int_update_tss, .-int_update_tss
	.globl	int_init
	.type	int_init, @function
int_init:
.LFB6:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$8, %esp
	call	init_interrupt
	call	setup_gdt
	call	setup_idt
	call	int_diags
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE6:
	.size	int_init, .-int_init
	.section	.rodata
.LC0:
	.string	"cr0: "
.LC1:
	.string	"---------------\n"
.LC2:
	.string	"\t protected mode enabled: "
.LC3:
	.string	"\t paging enabled: "
.LC4:
	.string	"\n"
.LC5:
	.string	"cr3: "
.LC6:
	.string	"cs: "
.LC7:
	.string	"esp: "
.LC8:
	.string	"a20: "
	.text
	.globl	int_diags
	.type	int_diags, @function
int_diags:
.LFB7:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	pushl	%ebx
	subl	$52, %esp
	.cfi_offset 3, -12
/APP
/  149 "int.c" 1
	movl %cr0, %ebx
/  0 "" 2
/NO_APP
	movl	%ebx, -12(%ebp)
	movl	$.LC0, (%esp)
	call	klib_print
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	klib_putint
	movl	$.LC1, (%esp)
	call	klib_print
	movl	$.LC2, (%esp)
	call	klib_print
	movl	-12(%ebp), %eax
	andl	$1, %eax
	movl	%eax, (%esp)
	call	klib_putint
	movl	$.LC3, (%esp)
	call	klib_print
	movl	-12(%ebp), %eax
	andl	$-2147483648, %eax
	movl	%eax, (%esp)
	call	klib_putint
	movl	$.LC4, (%esp)
	call	klib_print
/APP
/  160 "int.c" 1
	movl %cr3, %ebx
/  0 "" 2
/NO_APP
	movl	%ebx, -16(%ebp)
	movl	$.LC5, (%esp)
	call	klib_print
	movl	-16(%ebp), %eax
	movl	%eax, (%esp)
	call	klib_putint
/APP
/  164 "int.c" 1
	movl %cs, %ebx
/  0 "" 2
/NO_APP
	movl	%ebx, -20(%ebp)
	movl	$.LC6, (%esp)
	call	klib_print
	movl	-20(%ebp), %eax
	movl	%eax, (%esp)
	call	klib_putint
/APP
/  168 "int.c" 1
	movl %esp, %ebx
/  0 "" 2
/NO_APP
	movl	%ebx, -24(%ebp)
	movl	$.LC7, (%esp)
	call	klib_print
	movl	-24(%ebp), %eax
	movl	%eax, (%esp)
	call	klib_putint
	movl	$146, (%esp)
	call	read_port
	movzbl	%al, %eax
	movl	%eax, -28(%ebp)
	movl	$.LC8, (%esp)
	call	klib_print
	movl	-28(%ebp), %eax
	movl	%eax, (%esp)
	call	klib_putint
	addl	$52, %esp
	popl	%ebx
	.cfi_restore 3
	popl	%ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE7:
	.size	int_diags, .-int_diags
	.local	idt.1374
	.comm	idt.1374,2048,32
	.local	idt_desc.1375
	.comm	idt_desc.1375,8,4
	.ident	"GCC: (GNU) 4.7.2"
