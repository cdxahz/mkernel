#include "mm.h"
#include "klib.h"
#include "multiboot.h"

#define GDT_ADDRESS 0x10C000
#define PG0_ADDRESS 0x10D000
#define PG1_ADDRESS 0x10E000

static void mm_dump_phy(multiboot_info_t* mb);
static void mm_get_phy_mem_bound(multiboot_info_t* mb);
static void mm_setup_beginning_8m();

static unsigned long long phy_mem_low;
static unsigned long long phy_mem_high;



void mm_init(multiboot_info_t* mb)
{
    // mm_dump_phy(mb);
    mm_get_phy_mem_bound(mb);
    mm_setup_beginning_8m();
}



void mm_dump_phy(multiboot_info_t* mb)
{
    unsigned int count = mb->mmap_length;
    unsigned int map_addr = mb->mmap_addr;
    unsigned int i = 0;
    memory_map_t *map;
    klib_info("flags: ", mb->flags, "\n");

    klib_info("mem_lower: ", mb->mem_lower, "\t");

    klib_info("mem_upper: ", mb->mem_upper, "\t");

    klib_info("boot_device: ", mb->boot_device, "\n");

    klib_info("cmdline: ", mb->cmdline, "\t");

    klib_info("mods_count: ", mb->mods_count, "\t");

    klib_info("mods_addr: ", mb->mods_addr, "\n");

    klib_info("mmap_length: ", mb->mmap_length, "\t");

    klib_info("mmap_addr: ", mb->mmap_addr, "\n");

    if (mb->flags & 0x40) {
        
    map = mb->mmap_addr; 
        while((unsigned int)map < mb->mmap_addr + mb->mmap_length) {

            klib_info("base_addr: ", map->base_addr_low, ":");
            klib_info("", map->base_addr_high, "\t");
            klib_info("length: ", map->length_low, ":");
            klib_info("", map->length_high, "\t");
            klib_info("type: ", map->type, "\n");

    		map = (memory_map_t*) ( (unsigned int)map + map->size + sizeof(unsigned int) );
    	}
    }

    /* 
    dump info when set -m 256 
    flags: 0x4F                                                                     
    mem_lower: 0x27F    mem_upper: 0x3FBF8  boot_device: 0x8000FFFF                 
    cmdline: 0x109000   mods_count: 0x0 mods_addr: 0x109000                         
    mmap_length: 0x90   mmap_addr: 0x9000                                           
    base_addr: 0x0:0x0          length: 0x9FC00:0x0     type: 0x1  ; first 640K reserved for booting                            
    base_addr: 0x9FC00:0x0      length: 0x400:0x0       type: 0x2                           
    base_addr: 0xF0000:0x0      length: 0x10000:0x0     type: 0x2                           
    base_addr: 0x100000:0x0     length: 0xFEFE000:0x0   type: 0x1  ; usable memory                     
    base_addr: 0xFFFE000:0x0    length: 0x2000:0x0      type: 0x2                       
    base_addr: 0xFFFC0000:0x0   length: 0x40000:0x0     type: 0x2 
    */
}

static void mm_get_phy_mem_bound(multiboot_info_t* mb)
{  
    unsigned int map_addr = mb->mmap_addr;
    memory_map_t *map;

    if (mb->flags & 0x40) {
        
        map = (memory_map_t*)mb->mmap_addr; 
        while((unsigned int)map < mb->mmap_addr + mb->mmap_length) {

            if (map->type == 0x1 && map->base_addr_low != 0) {
                phy_mem_low = map->base_addr_low + map->base_addr_high << 32;
                phy_mem_high = phy_mem_low + (map->length_low + map->length_high << 32);
                break;
            }
    		map = (memory_map_t*) ( (unsigned int)map + map->size + sizeof(unsigned int) );
    	}
    }

     // klib_info("static address: ", &phy_mem_low, "\n");
     // klib_info("local address: ", &map_addr, "\n");
     // klib_info("code address: ", &mm_get_phy_mem_bound, "\n");
}

#define RELOAD_CR3(address) \
    __asm__ ("movl %0, %%cr3" : : "q" (address));

#define ENABLE_PAGING() \
    __asm__ ("movl %cr0,%eax"); \
    __asm__ ("orl $0x80000000,%eax"); \
    __asm__ ("movl %eax,%cr0");

#define RELOAD_EIP() \
__asm__ ("jmp 1f \n1:\n\tmovl $1f,%eax\n\tjmp *%eax \n1:\n\tnop");


static void simulate_paging(unsigned address)
{
    int *gdt = (int*)GDT_ADDRESS;
    int *pet = 0;
    int gd_index = 0;
    int pe_index = 0;
    int page_offset = 0;
    unsigned phy = 0;

    klib_info("virtual address: ", address, "\n");

    gd_index = ADDR_TO_PGT_OFFSET(address) ;
    pe_index = ADDR_TO_PET_OFFSET(address) ;
    page_offset = ADDR_TO_PAGE_OFFSET(address);

    klib_info("gd_index: ", gd_index, "\n");
    klib_info("pe_index: ", pe_index, "\n");
    klib_info("page_offset: ", page_offset, "\n");

    pet = (int*)(gdt[0] & 0xfffff000);
    klib_info("pet: ", pet, "\n");

    phy = pet[pe_index] & 0xfffff000;
    klib_info("phy: ", phy, "\n");

    phy = phy | page_offset;
    klib_info("physical address: ", phy, "\n");
}
static void mm_setup_beginning_8m()
{
    unsigned int phy = 7;
    int i = 0;
    int * gdt = (int*)GDT_ADDRESS;
    int * pg0 = (int*)PG0_ADDRESS;
    int * pg1 = (int*)PG1_ADDRESS;
    for (i = 0; i < PG_TABLE_SIZE; i++) {
        gdt[i] = 0;
    }
    // following two are for user space
    gdt[0] = PG0_ADDRESS | 0x07; 
    gdt[1] = PG0_ADDRESS | 0x07;

    // following two are for kernel space
    gdt[768] = PG0_ADDRESS | 0x07;
    gdt[769] = PG0_ADDRESS | 0x07;

    for (i = 0; i < PE_TABLE_SIZE; i++) {
        pg0[i] = phy;
        phy += PAGE_SIZE;
    }

    for (i = 0; i < PE_TABLE_SIZE; i++) {
        pg1[i] = phy;
        phy += PAGE_SIZE;
    }

    RELOAD_CR3(GDT_ADDRESS);
    ENABLE_PAGING();
    RELOAD_EIP();
    // simulate_paging(0x001012a0);
}
