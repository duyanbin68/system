
/************************************************************************/
/* 此文件主要实现Windows窗口的创建、显示
叠加，剪切，移动。在内核中运行，由单一进程调用，从而实现窗口快速调用。
*/
/************************************************************************/
#include "windows.h"
#include "gdi.h"
#include "../driver/mouse.h"
#include "../mm/memory.h"

LPThread * active_window;
char wid = 1;
//全局位图,需要刷新的位图
char * windows_map;

/************************************************************************/
/* 初始化windows窗口
                                                                        */
/************************************************************************/
void windows_init(){
     windows_map = (char *)alloc_mem(screen_width * screen_height * 2);
     
}
/************************************************************************/
/* 创建窗口主要功能，分配内存给要显示的窗口
建立窗口双向链表，在内核中运行
*/
/************************************************************************/
int create_window(){


	return 0;
}

/************************************************************************/
/*窗口叠加处理，从底到顶依次扫描，鼠标滑过的部分，并刷新
假如拖动窗体，那么先刷新鼠标部分，再刷新窗体部分
*/
/************************************************************************/

int sys_windows_refresh(int x,int y,int mx,int my){
	char * tmpbuf;
	int i,j;
    	struct task_window *s;
    	RECT r2,r3,r4;
	
	//从底置顶
    	for(s = head;s->next;s=s->next){
		//第2-n个窗口在屏幕中的区域
		r3.left = s->x >0 ? s->x : 0;
		r3.top = s->y > 0 ? s->y : 0;
		r3.right = s->right < screen_width ? s->right:screen_width;
		r3.bottom = s->bottom < screen_height ? s->bottom:screen_height;
		//顶层窗口在屏幕中的区域
		r4.left = tail->x >0 ? tail->x : 0;
		r4.top = tail->y > 0 ? tail->y : 0;
		r4.right = tail->right < screen_width ? tail->right : screen_width;
		r4.bottom = tail->bottom < screen_height ? tail->bottom : screen_height;
		//相交得出移动顶层窗口后需要重画的区域(移动窗口前重叠区域)
		if(sys_rect_intersect(&s->rects,&r3,&r4))
		{       //将需要重画的区域位图置id，坐标可能需要转换
			sys_writemap(&s->rects,windows_map,s->id);
		}
		
	}
	//更新顶层窗口移动后的区域，可以优化省略r1
	tail->x += mx;
	tail->y += my;
	tail->right += mx;
	tail->bottom += my;
	
	r2.left = tail->x;
	r2.top = tail->y;
	r2.right = tail->right;
	r2.bottom = tail->bottom;

	sys_writemap(&r2,(char *)windows_map,0);
	
	for( s = head; s->next ;s =s->next){
		
		tmpbuf = windows_map;
		//根据位图重画
		for (i = s->rects.top; i< s->rects.bottom;i++)
		{
			for (j = s->rects.left;j < s->rects.right;j++)
			{
				if (tmpbuf[i*screen_width+j] == s->id)
				{
					draw_window_point(j,i,get_point_color(j-s->x,i-s->y,(void *)s->buf,s->width),
						(void *)vesa_start,screen_width);
				}
				
			}
		}   
	}
	
	//画顶层窗口
	sys_bitblt(&r2,tail);

	return 0;
}


/************************************************************************/
/*刷新鼠标
*/
/************************************************************************/

int sys_mouse_refresh(int x,int y,int oldx,int oldy){
	char * tmpbuf;
	int i,j,x1,y1;
    struct task_window *s;
    RECT r1,r2,r3;

	x1 = oldx + 11;
	y1 = oldy + 19;

	//从顶置底
    for(s = head;s;s=s->next){
		//第2-n个窗口在屏幕中的区域
		r2.left = s->x >0 ? s->x : 0;
		r2.top = s->y > 0 ? s->y : 0;
		r2.right = s->right < screen_width ? s->right:screen_width;
		r2.bottom = s->bottom < screen_height ? s->bottom:screen_height;
		//鼠标在屏幕中的区域
		r3.left = oldx > 0 ? oldx : 0;
		r3.top = oldy > 0 ? oldy : 0;
		r3.right = x1 < screen_width ? x1 : screen_width;
	    r3.bottom = y1 < screen_height ? y1 : screen_height;

		//相交得出移动鼠标后需要重画的区域
		if(sys_rect_intersect(&s->rects,&r2,&r3))
		{       //将需要重画的区域位图置id，坐标可能需要转换
			sys_writemap(&s->rects,windows_map,s->id);
		}
	}
	//更新鼠标
	r1.left = x;
	r1.top = y;
	r1.right = x + 11;
	r1.bottom = y + 19;
	
	//先画鼠标，函数
    sys_draw_mouse_icon(x,y,(u16*)__arrow);

	for( s = head; s ;s =s->next){
		
		tmpbuf = windows_map;
		//根据位图重画
		for (i = s->rects.top; i< s->rects.bottom;i++)
		{
			for (j = s->rects.left;j < s->rects.right;j++)
			{
				if (tmpbuf[i*screen_width+j] == s->id)
				{
					draw_window_point(j,i,get_point_color(j-s->x,i-s->y,(void *)s->buf,s->width),
						(void *)vesa_start,screen_width);
				}
				
			}
		}   
	}

    //尝试后画鼠标
	sys_draw_mouse_icon(x,y,(u16*)__arrow);
	return 0;

}
int sys_bring_window_to_top(struct task_window *p){
	
	struct task_window *tmp; 
	//由于head，tail是全局变量，属于内核，所以要内陷到内核态去保存
	if( p!=tail){
		//让p指向下一节点
		//
		p->previous->next = p->next;
		p->next->previous = p->previous;
		
		tail->next = p;
		tmp = tail;
		
		tail = p;	
		
		tail->previous = tmp;
		tail->next = 0;	
		
		return 1;
	}
	
	return 0;
}
/************************************************************************/
/* 写位图
*/
/************************************************************************/
int sys_writemap(PRECT r,char * bmp,char b){
	
	int p_offset,x,y;
		
	for ( y = r->top;y < r->bottom; y++)
	{
		for( x = r->left ;x < r->right; x++){
			
			p_offset = y * screen_width + x;
		
			*(bmp + p_offset) = b;

		}
	}
	
	return 0;
	
}

/************************************************************************/
/*为鼠标透明部分专门写一个位图函数
                                                                      */
/************************************************************************/
int sys_mousewritemap(PRECT r,char * bmp,char b){
	
	int p_offset,q_offset = 0,x,y;
	
	for ( y = r->top;y < r->bottom; y++)
	{
		for( x = r->left ;x < r->right; x++){
			
			p_offset = y * screen_width + x;
			
            if(__arrow [q_offset] != -1)	
				*(bmp + p_offset) = b;

			q_offset++;
		}
	}
	
	return 0;
	
}

int sys_rect_intersect(RECT* pdrc, const RECT* psrc1, const RECT* psrc2)
{  
	//求两个RECT的相交的部分，也是一个RECT
	pdrc->left = (psrc1->left > psrc2->left) ? psrc1->left : psrc2->left;
	pdrc->top  = (psrc1->top > psrc2->top) ? psrc1->top : psrc2->top;
	pdrc->right = (psrc1->right < psrc2->right) ? psrc1->right : psrc2->right;
	pdrc->bottom = (psrc1->bottom < psrc2->bottom)? psrc1->bottom : psrc2->bottom;
	if(pdrc->left >= pdrc->right || pdrc->top >= pdrc->bottom)
		return FALSE;
	return TRUE;
}

int sys_bitblt(PRECT r,struct task_window * t){
	
	
	int i,j,offset,w_offset,y,x;
	u8 * p1;u16 * p2;u32 * p3;
	p1 = (u8 *)t->buf;
	p2 = (u16 *)t->buf;
	p3 = (u32 *)t->buf;
	
	x = t->x;
	y = t->y;

	
	for ( i = y; i < y + t->height;i++)
    {
		for (j = x; j < x + t->width;j++)
		{
			if(i<0 || i > screen_height-1 ||j <0 || j>screen_width-1)
				continue;
            offset = i * screen_width + j;
			w_offset = (i-y) * t->width + (j-x);

			if(color_bit == 8)
				*((u8 *)vesa_start + offset) = *(p1 + w_offset);
			else if(color_bit == 32)
				*((u32 *)vesa_start + offset) = *(p3 + w_offset);
			else
				*((u16 *)vesa_start + offset) = *(p2 + w_offset);
			
		}
    }
    
	return 1;
}

//取点函数
u32 get_point_color(int x,int y,void *buf,int width)
{
	
	int p_offset = 0;
	u32 ret_val = 0;
    u8 * p1;
	u16 * p2;
	u32 * p3;
	p1 = (u8 *)buf;
	p2 =  (u16 *)buf;
	p3 = (u32 *)buf;
	
	if(x<0) return 0;
	if(y<0) return 0;
	if(x >screen_width ) return 0;
	if(y >screen_height) return 0;
	
	p_offset = y * width +x;
	if(color_bit == 8)
	{
		ret_val = (u32)*(p1 + p_offset);
	}
	else if(color_bit == 32){
        ret_val = *(p3 + p_offset);
	}
	else
	{
		ret_val = (u32)*(p2 + p_offset);
	}
	return ret_val;
}
//画窗口函数，在内核态运行
int sys_CreateWindow(PRECT r){
	
	int i,j;
	unsigned short * p;
	void * buf;
	struct task_window *t;
	char str[60]={0};
	//在内存中分配一个windows结构体
	//t = (struct task_window *)alloc_mem(sizeof(struct task_window));
	
	current->twin.hwnd = (HWND)&current->twin;
	t = (LPTASKWINDOW)&current->twin;
	
	if(t == 0){
		draw_engchar(300,200,"memory wrong",0x7f00,(void *)vesa_start,screen_width);	
		for(;;);
	}
	t->width = r->right - r->left;
	t->height = r->bottom - r->top;
	t->x = r->left;
	t->y = r->top;
    t->right = r->right;
    t->bottom = r->bottom;
    t->id = getwid();

	//sprintf(str,"%d",t->id);
	//debug_printb(str);
	t->buf = alloc_mem(t->width * t->height * color_bit  / 8 * 2);
	if(t->buf == 0){
			draw_engchar(300,200,"memory wrong",0x7f00,(void *)vesa_start,screen_width);
		for(;;);
	}
	buf = (void *)t->buf;
	//插入链表

	t->next = 0;
	t->previous = tail;
	tail->next = t; //链入尾部
	tail = t; //保存尾节点

	low_draw_rec(0,0,t->width-1,0,COLOR_C6C6C6,t->width,buf);
	low_draw_rec(1,1,t->width-2,0+1,COLOR_FFFFFF,t->width,buf);
	low_draw_rec(0,0,0,t->height-1,COLOR_C6C6C6,t->width,buf);
	low_draw_rec(1,1,0+1,t->height-2,COLOR_FFFFFF,t->width,buf);
	low_draw_rec(t->width-2,0+1,t->width-2,t->height-2,COLOR_848484,t->width,buf);
	low_draw_rec(t->width-1,0,t->width-1,t->height-1,0,t->width,buf);
	low_draw_rec(2,2,t->width-3,t->height-3,COLOR_C6C6C6,t->width,buf);
	if(color_bit == 16)
	low_draw_rec(3,3,t->width-4,20,COLOR_000015,t->width,buf);
	else
    draw_window_bar(3,3,t->width-4,20,COLOR_000015,t->width,buf);

	low_draw_rec(1,t->height-1,t->width-2,t->height-2,COLOR_848484,t->width,buf);
	low_draw_rec(0,t->height-1,t->width-1,t->height-1,0,t->width,buf);
	
	draw_engchar(14,4,"Window",COLOR_FFFFFF,buf,t->width);
	//画关闭图标	
	p = (unsigned short*)&_closebtn;

	for(j=5;j<19;j++){
		for (i=t->width-21;i<t->width-5;i++)
		{
			if (15 == *p)
			{
				draw_window_point(i,j,COLOR_FFFFFF,buf,t->width);
			} 
			else if(*p == 7)
			{
				draw_window_point(i,j,COLOR_C6C6C6,buf,t->width);
			}else if(*p == 14){
                draw_window_point(i,j,COLOR_848484,buf,t->width);
			}else{
				draw_window_point(i,j,0,buf,t->width);
			}
			p++;
		}
	}
	//最大化图标
	
	p = (unsigned short*)&_maxbtn;
	
	for(j = 5;j < 19;j++){
		for (i=t->width-39;i<t->width-23;i++)
		{
			if (15 == *p)
			{
				draw_window_point(i,j,COLOR_FFFFFF,buf,t->width);
			} 
			else if(*p == 7)
			{
				draw_window_point(i,j,COLOR_C6C6C6,buf,t->width);
			}else if(*p == 14){
                draw_window_point(i,j,COLOR_848484,buf,t->width);
			}else{
				draw_window_point(i,j,0,buf,t->width);
			}
			p++;
		}
	}
	//最小化图标
	p = (unsigned short*)&_minbtn;

	for(j=5;j<19;j++){
		for (i=t->width-57;i<t->width-41;i++)
		{
			if (15 == *p)
			{
				draw_window_point(i,j,COLOR_FFFFFF,buf,t->width);
			} 
			else if(*p == 7)
			{
				draw_window_point(i,j,COLOR_C6C6C6,buf,t->width);
			}else if(*p == 14){
                draw_window_point(i,j,COLOR_848484,buf,t->width);
			}else{
				draw_window_point(i,j,0,buf,t->width);
			}
			p++;
		}
	}

	//draw_listbox(5,25,t->width- 11,t->height- 31,t->width,(void *)t->buf);
	//sys_invalidate((u32)t);

	sprintf(str,"the windows is %x\n",t);
	debug_printb(str);
	
	return (int)t;
}


int sys_create_console(PRECT r){
	int hwnd;
	struct task_window *t;
	char buf[120]={0};

	hwnd = sys_CreateWindow(r);

	t = (struct task_window *)hwnd;
	
	draw_listbox(5,25,t->width- 11,t->height- 31,t->width,(void *)t->buf);

	draw_engchar(8,23,"C:\\>",0xc618,(void *)t->buf,t->width);

	sys_invalidate(hwnd);
	
	return hwnd;
}

int sys_invalidate(u32 hWnd){
	
	struct task_window *t;
	int i,j;

	t = (struct task_window *)hWnd;

    //把整个窗口画到桌面上。
	for(i = t->y;i < t->bottom;i++)
		for (j = t->x;j < t->right;j++)
			draw_window_point(j,i,get_point_color(j - t->x,i - t->y,(void *)t->buf,t->width),(void *)vesa_start,screen_width);
	return 0;
	
}

int sys_invalidateRect(HWND hWnd,PRECT rect){
	
	struct task_window *t;
	int i,j;

	t = (struct task_window *)hWnd;

    //把整个窗口画到桌面上。
	for(i = rect->top;i < rect->bottom;i++)
		for (j = rect->left;j < rect->right;j++)
			draw_window_point(j,i,get_point_color(j - t->x,i - t->y,(void *)t->buf,t->width),(void *)vesa_start,screen_width);
	return 0;
	
}

int sys_ScrollScreen(HWND hWnd){
	
	struct task_window *t;
	int i,j;

	t = (struct task_window *)hWnd;

	//printk("x,y is %d,%d",t->x,t->y);
	  for(i = 0 + 23 + 16;i < t->height - 30;i++){
		  for (j = 0 + 10;j < t->width - 30;j++){
		  		draw_window_point(j,i - 16,0,(void *)t->buf,t->width);
				draw_window_point(j,i - 16,get_point_color(j,i,(void *)t->buf,t->width),(void *)t->buf,t->width);
		  	}
	  }
	  for(i = t->height - 5 - 32;i < t->height - 5;i++){
			   for (j = 10;j < t->width - 30;j++){
			   	draw_window_point(j,i,0,(void *)t->buf,t->width);
		   	}
	  }

	return 0;
	
}

