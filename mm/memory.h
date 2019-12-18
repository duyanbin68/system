#ifndef _MEMORY_H
#define _MEMORY_H
typedef unsigned int UINT;
typedef int size_t;
typedef unsigned long  ULONG;
//内存管理结构
typedef struct tag_memory_manage{
	//memory addr
	unsigned long addr;
	//memory size
	unsigned long size;
	//count
	int refer_count;
	//piont to next&prev struct
	struct tag_memory_manage *next;
	struct tag_memory_manage *prev;

}MEMORY_MANAGE,*lpMemory_manage;

typedef struct{
    unsigned long id;
    unsigned long addr;
}virtual_man;

#define head4K 0x10000
#define k_node 3
#define LOW_MEM 0x400000
#define DEC_MEM_COUNT 0
#define INC_MEM_COUNT 1
#define INQ_MEM_COUNT 2
#define COPY_PAGE_NUMBER 128

extern void memory_init();

extern int check_memory(unsigned long start, unsigned long end);


extern unsigned long put_page(unsigned long page,unsigned long address,unsigned long cr3);
extern void copy_page(unsigned long from, unsigned long to);
extern void un_wp_page(unsigned long * table_entry);
extern void set_video_pages(long cr3);
extern void copy_memory(void * dest,void * src,unsigned int count);
//extern int copy_page_tables(struct task_struct * tsk);

extern void init_memory(unsigned long start_addr,unsigned long size);
extern unsigned long alloc_mem(unsigned long size);
extern int free_mem(unsigned long addr);
extern int modify_mem_refcount(unsigned long addr,int fbool);
extern unsigned int alloc_mem4k();
extern int virtual_to_physical(UINT cr3,UINT line_addr,UINT size,UINT phy_addr,UINT tmp);
extern void *kmemcpy(void * to, const void * from, size_t n);

#endif
