#include "memory.h"
//内存空闲头，空闲尾，已用的头
lpMemory_manage mem_free_head,mem_free_tail,mem_used_head;
//记录内存总数
unsigned long memory_total;

int memory_count = 0;
//记录分配4k内存的数组
char * mem_arry;
void memory_init(){
	  memory_total = *(unsigned long *)0x9000a;

      	  init_memory(0x400000,memory_total - 0x400000);

	  mem_arry = (char *)(0x400000 - 0x1000);

	 // memset(mem_arry,0,0x1000);
}
//映射4M内存，类似put_page
int virtual_to_physical(UINT cr3,UINT line_addr,UINT size,UINT phy_addr,UINT tmp)
{
	UINT i;
	UINT *dir,*table;
	if(!tmp)
		return 0;
	//得到页目录表中此项的地址
	dir = (UINT *)(cr3 + (line_addr >> 20));
	//4M向上取整，
	size = (size + 0x3fffff) >> 22;
	//填写整个4M
	for(i = 0; i < size; i++,dir++){
		*dir = tmp | 7; //tmp是为页表分配的内存，大小应为4K，可以容纳1024项，把地址写入页目录表中
		for(table = (UINT*)tmp; table < (UINT*)(tmp + 0x1000);table++,phy_addr += 0x1000)
		{
			//填写这1024项，每一项指向一段4K的内存，共4M
			*table = phy_addr | 7;
		}
		tmp += 0x1000;
	}
	return 1;
}

/*
cr3的值是0x80000，意味着页目录放在了0x80000
地址0x80000内容如下
页目录表 0x80000: 指向地址0的页表项
       0x80004:

	某一项：(line_addr >> 20)

line_addr >> 20
页表： tmp | 7

*/

/*memcpy */
void * kmemcpy(void * to, const void * from, size_t n)
{
	asm("cld;"
	"movl %%edx, %%ecx\n\t"
	"shrl $2,%%ecx\n\t"
	"rep ; movsl\n\t"
	"testb $1,%%dl\n\t"
	"je 1f\n\t"
	"movsb\n"
	"1:\ttestb $2,%%dl\n\t"
	"je 2f\n\t"
	"movsw\n"
	"2:\n"
	: /* no output */
	:"d" (n),"D" ((long) to),"S" ((long) from)
	);
return (to);
}

/*
	分配4K内存，专门用于分页，低12位必须是0
	物理内存地址3M-4M用于分页内存
*/
unsigned int alloc_mem4k(){
	int i;
	unsigned int retval = 0;

	for(i = 0; i < 0x1000; i++){
		if( mem_arry[i] == 0){
			retval = 0x300000 + i * 0x1000;
			mem_arry[i] = 1;
			break;
		}
	}

	return retval & 0xfffff000;
}

int free_mem4k(unsigned int addr){

	int i = 0;

	if(addr > 0x400000 - 0x1000 || addr < 0x300000)
		return 0;
	i = (addr - 0x300000) / 0x1000;

	if( mem_arry[i] == 1){
		mem_arry[i] = 0;
		return 1;
		}

	return 0;
}
//分配任意大小内存，链表初始化
void init_memory(unsigned long start_addr,unsigned long size)
{
	int i;

	lpMemory_manage p;

    	p = mem_free_head  = (lpMemory_manage)head4K;
    	mem_used_head = (lpMemory_manage)0;
    	//
    	mem_free_head->addr = start_addr;
    	mem_free_head->size = size;
    	mem_free_head->refer_count = 0;
    	mem_free_head->prev = 0;
    	mem_free_head->next = 0;

    	//
    	for(i = 0;i < 4096 / sizeof(MEMORY_MANAGE) - 1;i++)
        {
	        p->next = p + 1;
    		p++;
    		p->prev = p - 1;

    		p->addr = 0;
    		p->size = 0;
    		p->refer_count = 0;
    	}
    	//
		mem_free_tail = p;
		p->next = 0;
}
//
int check_node(){

    int n = 0;
    lpMemory_manage p;

    for(p = mem_free_head; p; p = p->next){
          if(p->addr==0 && p->size==0)
            n++;
    }
    return(n < k_node);
}
/************************************************************************/
/*                                        */
/************************************************************************/
unsigned long alloc_mem(unsigned long size)
{
     int retval = 0;
     int i;
     lpMemory_manage p,q;
     //
	 memory_count++;
	
     for(p = mem_free_head; p->next || p->addr; p = p->next){
        if(p->size >= size){
           //
           retval = p->addr;
           //
           p->refer_count++;
		
		   if(p->size > size){
			   
			   for(q = p->next;q;q = q->next){
				  
				   if (q->addr == 0 && q->size == 0)
				   {
					   q->addr = p->addr + size;
					   q->size = p->size - size;
					   break;
				   }
			   }
		   }
        
           p->size = size;

           if(p == mem_free_head){
                mem_free_head = p->next;
				mem_free_head->prev = 0;
           }else{
                p->prev->next = p->next;
                p->next->prev = p->prev;
           }
           if(mem_used_head == 0){
              mem_used_head = p;
              mem_used_head->prev = 0;
              mem_used_head->next = 0;
           }else{
           p->next = mem_used_head;
           mem_used_head->prev = p;

           mem_used_head = p;
           mem_used_head->prev = 0;
           }
           break;
        }
     }




     if(check_node()){
      //检查节点数
		 for(q = mem_free_head;q->next;q = q->next);

         q->next = p = (lpMemory_manage)alloc_mem4k();
         p->prev = q;

		 p->addr = 0;
		 p->size = 0;
    	 p->refer_count = 0;

       	for(i = 0;i < 256 / sizeof(MEMORY_MANAGE) - 1;i++)
        {
            p->next = p + 1;
    		p++;
    		p->prev = p - 1;

    		p->addr = 0;
    		p->size = 0;
    		p->refer_count = 0;
		}
		p->next = 0;

     }
     if (retval == 0)
     {
		// debug_printb("memory alloc failed!\n");
     }
     return retval;
}
/************************************************************************
释放内存函数
参数，addr，要释放的地址
成功返回-1，失败返回0
************************************************************************/
int free_mem(unsigned long addr)
{
	lpMemory_manage p;
	   //如果地址为0，返回0
	if(!addr)
		return 0;
	   //从忙链遍历
	for(p = mem_used_head; p ;p = p->next){
		if(p->addr == addr){
			//如果找到地址，引用数清0
			p->refer_count = 0;
			//如果前面和后面都是空，那么指针清0
			if(p->prev == 0 && p->next == 0){

				mem_used_head = 0;
            //
			}else if(p->prev == 0){

				mem_used_head = p->next;
				mem_used_head->prev = 0;
            //
			}else if(p->next == 0){

				p->prev->next = 0;
            //
			}else{
				p->prev->next = p->next;
				p->next->prev = p->prev;
			}
			//
			mem_free_head->prev = p;
			p->next = mem_free_head;
			mem_free_head = p;

			//
			return -1;
		}
	}
	return 0;
}
void copy_page(unsigned long from, unsigned long to){
	
	asm("pushf;"
	    "mov $1024,%%ecx;"
	    "cld;"
	    "rep; movsl\n\t"
	    "popf\n\t"
	::"S"(from),"D"(to));
}

inline void inva_page(){
	asm("mov %cr3, %eax\n mov %eax, %cr3\n");
}
