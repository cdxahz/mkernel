#ifndef _MM_H_
#define _MM_H_

typedef struct multiboot_info multiboot_info_t;

#define PAGE_SIZE (4*1024)

#define PG_TABLE_SIZE 1024
#define PE_TABLE_SIZE 1024

#define OFFSET_IN_PAGE_MASK 0x00000FFF
#define OFFSET_IN_PET_MASK  0x003FF000
#define OFFSET_IN_PGT_MASK  0xFFC00000

#define ADDR_TO_PGT_OFFSET(addr)  ((addr & OFFSET_IN_PGT_MASK) >> 22)
#define ADDR_TO_PET_OFFSET(addr)  ((addr & OFFSET_IN_PET_MASK) >> 12)
#define ADDR_TO_PAGE_OFFSET(addr) (addr & OFFSET_IN_PAGE_MASK)

void mm_init(multiboot_info_t* mb);

#endif
