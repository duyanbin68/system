#include "gdi.h"
#include "ASC8X16.h"

extern unsigned char color_bit;
extern int screen_width;
extern int screen_height;
extern unsigned int * vesa_start;

short _closebtn[] = {
	    	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,1,1,
		1,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,1,1,
		1,7,7,7,7,7,0,0,7,7,7,7,0,0,7,7,1,1,1,1,
		1,7,7,7,7,7,7,0,0,7,7,0,0,7,7,7,1,1,1,1,
		1,7,7,7,7,7,7,7,0,0,0,0,7,7,7,7,1,1,1,1,
		1,7,7,7,7,7,7,7,7,0,0,7,7,7,7,7,1,1,1,1,
		1,7,7,7,7,7,7,7,0,0,0,0,7,7,7,7,1,1,1,1,
		1,7,7,7,7,7,7,0,0,7,7,0,0,7,7,7,1,1,1,1,
		1,7,7,7,7,7,0,0,7,7,7,7,0,0,7,7,1,1,1,1,
		1,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,1,1,
		1,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
};

short _maxbtn[] = {
	        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,7,7,7,7,7,7,7,7,7,7,7,7,7,1,1,
		1,7,7,0,0,0,0,0,0,0,0,0,7,7,1,1,
		1,7,7,0,0,0,0,0,0,0,0,0,7,7,1,1,
		1,7,7,0,7,7,7,7,7,7,7,0,7,7,1,1,
		1,7,7,0,7,7,7,7,7,7,7,0,7,7,1,1,
		1,7,7,0,7,7,7,7,7,7,7,0,7,7,1,1,
		1,7,7,0,7,7,7,7,7,7,7,0,7,7,1,1,
		1,7,7,0,7,7,7,7,7,7,7,0,7,7,1,1,
		1,7,7,0,7,7,7,7,7,7,7,0,7,7,1,1,
		1,7,7,0,0,0,0,0,0,0,0,0,7,7,1,1,
		1,7,7,7,7,7,7,7,7,7,7,7,7,7,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
};

short _minbtn[] = {
	    	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,7,7,7,7,7,7,7,7,7,7,7,7,7,1,1,
		1,7,7,7,7,7,7,7,7,7,7,7,7,7,1,1,
		1,7,7,7,7,7,7,7,7,7,7,7,7,7,1,1,
		1,7,7,7,7,7,7,7,7,7,7,7,7,7,1,1,
		1,7,7,7,7,7,7,7,7,7,7,7,7,7,1,1,
		1,7,7,7,7,7,7,7,7,7,7,7,7,7,1,1,
		1,7,7,7,7,7,7,7,7,7,7,7,7,7,1,1,
		1,7,7,7,7,7,7,7,7,7,7,7,7,7,1,1,
		1,7,7,0,0,0,0,0,0,0,0,0,7,7,1,1,
		1,7,7,0,0,0,0,0,0,0,0,0,7,7,1,1,
		1,7,7,7,7,7,7,7,7,7,7,7,7,7,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	};

void draw_engchar(int x,int y,char * s,u32 color,u32 * buffer,int width){
	int i,k;
	char * hoffset;
	
	while(*s){
		
		hoffset = szASC8X16 + 16 * (int)*s;
		
		for(i=0;i<16;i++)
		{	
			for(k=0;k<8;k++){
				if(((hoffset[i]>>(7-k))&0x1)!=0)
					draw_window_point(x+k,y+i,color,buffer,width);
			}
			
		}
		x += 8;
		s += 1;
	}
	
}

void draw_char(int x,int y,char s,u32 color,u32 * buffer,int width){
	int i,k;
	char * hoffset;
	
	hoffset = szASC8X16 + 16 * (int)s;
		
	for(i=0;i<16;i++)
	{	
		for(k=0;k<8;k++){
			if(((hoffset[i]>>(7-k))&0x1)!=0)
				draw_window_point(x+k,y+i,color,buffer,width);
		}
			
	}
	
}

/************************************************************************/
/* ע�ͣ� ��Ļ���㺯�� 
   �������� draw_window_point 
   ������   x,y  Ҫ��������꣬color ��ɫֵ��buf ��Ӧ�ڴ滺���ַ
            width ���ڵĿ��                                                          */
/************************************************************************/
int draw_window_point(int x,int y,u32 color,void * video,int width)
{
	u8  * buf8  = (u8 *)video;
	u16 * buf16 = (u16 *)video;
	u32 * buf32 = (u32 *)video;

	int p_offset = 0;

	if(y > screen_height -1 || x >screen_width -1)
		return 0;
	//�����λ��
	p_offset = y * width + x;
	//����
	if(color_bit == 8){
	
		*(buf8 + p_offset) = (u8)color;
	}
	else if(color_bit == 32){
		*(buf32 + p_offset) = (u32)color;
	}
	else{
	    *(buf16+ p_offset) = (u16)color;
	}

	return 1;
}

/************************************************************************/
/* ע�ͣ� ����������ͼ�ο�������Ļ��
������:   copy_window_to_screen
������    x,y ��������Ļ�ϵ�λ������
          buf ���ڻ����ַ
          width,height���ڿ�Ⱥ͸߶�
                                                                        */
/************************************************************************/

int copy_window_to_screen(int x, int y, void * buf,int width,int height){
    
	int i,j,offset;
	u32 * p;
	p = (u32 *)buf;


	for ( i = y; i < y + height;i++)
    	{
		for (j = x; j < x + width;j++)
		{
			draw_window_point(j,i,get_point_color(j - x,i - y,(void *)buf , width),(void *)vesa_start,screen_width);
		}
    	}
    
	return 1;
}

/************************************************************************/
/*ע�ͣ�  �ײ㻭���κ�������ʱ���������½ǵ㣬��Ч��
 ��������low_draw_rec
 ����:   x1,y1���Ͻǵ㣬x2,y2���½ǵ㣬color��ɫֵ,width���ڿ��,bufͼ�λ���
 
                                                              */
/************************************************************************/

int low_draw_rec(int x1,int y1,int x2,int y2,u32 color,int width,void * buf){
    int i,j;

	for (i = y1;i < y2 + 1 ;i++)
	{
		for (j = x1; j < x2 + 1;j++)
		{
          		draw_window_point(j,i,color,buf,width);
		}
	}

	return 0;
}


/************************************************************************/
/* ���������εĺ���������������Ƶ�
������: inter_draw_rec
����:x1,y1 ���Ͻǣ�x2,y2���½ǣ�color1��color2��ɫֵ��width��Ļ��ȣ�buf������*/
/************************************************************************/
int inter_draw_rec(int x1,int y1,int x2,int y2,u32 color1,u32 color2,int width,void * buf){
    	
	int i,j;
	u32 color;

	for (i = y1;i < y2 + 1 ;i++)
	{
		for (j = x1; j < x2 + 1;j++)
		{
			if ((j + i) % 2 == 0){
				color = color1;
			}
			else{
				color = color2;
			}
			draw_window_point(j,i,color,buf,width);
		}
	}
	
	return 0;
}

/************************************************************************/
/* ����ť����
������: draw_button
������x,y���Ͻ����ꣻwidth��ȣ�height�߶ȣ�window_width���ڿ�ȣ�buf������*/
/************************************************************************/

int draw_button(int x,int y,int width,int height,int window_width, void *buf){
    //����ť����ɫ
	low_draw_rec(x + 1, y, x + width + 1,y + height,COLOR_C6C6C6,window_width, buf);//Windows�����ɫ
	//����ť�߿�
	low_draw_rec( x + 1, y, x + width, y,COLOR_FFFFFF,window_width,buf);//������ɫ
	low_draw_rec( x , y,  x, y + height-1,COLOR_FFFFFF,window_width,buf);//������ɫ
	low_draw_rec( x , y + height-1, x + width, y + height-1,COLOR_848484,window_width,buf);//����ɫ
	low_draw_rec( x + width,y + 1, x + width, y +height-2,COLOR_848484,window_width,buf);
	
	low_draw_rec( x,y + height, x + width, y + height,COLOR_000000,window_width,buf);//��ɫ
	low_draw_rec( x + width + 1,y, x + width + 1, y + height,COLOR_000000,window_width,buf);//��ɫ
	
	return 0;
}

int draw_radio(int x,int y){
	
	return 0;
}

int draw_checkbox(int x,int y){
	return 0;
}

int draw_textbox(int x,int y,int width,int height,int window_width,void * buf){

	int x1,y1;
	x1 = x + width;
	y1 = y + height;

	low_draw_rec(x - 2, y - 3, x1 + 1,y1 - 3,COLOR_848484,window_width,buf);
	low_draw_rec(x - 3, y - 3, x  - 3,y1 + 1,COLOR_848484,window_width,buf);
	low_draw_rec(x - 3, y1 +2, x1 + 1,y1 + 2,COLOR_FFFFFF,window_width,buf);
	low_draw_rec(x1 +2, y - 3, x1 + 2,y1 + 2,COLOR_FFFFFF,window_width,buf);
	low_draw_rec(x - 1, y - 2, x1 + 0,y - 2,COLOR_000000,window_width,buf);
	low_draw_rec(x - 2, y - 2, x -  2,y1 + 0,COLOR_000000,window_width,buf);
	low_draw_rec(x - 2, y1 +1, x1 + 0,y1 + 1,COLOR_C6C6C6,window_width,buf);
	low_draw_rec(x1 +1, y - 2, x1 + 1,y1 + 1,COLOR_C6C6C6,window_width,buf);
	low_draw_rec(x - 1, y - 1, x1 + 0,y1 + 0,COLOR_FFFFFF,window_width,buf);
	return 0;
}

int draw_textbox_color(int x,int y,int width,int height,int window_width,void * buf,u32 color){

	int x1,y1;
	x1 = x + width;
	y1 = y + height;

	low_draw_rec(x - 2, y - 3, x1 + 1,y1 - 3,COLOR_848484,window_width,buf);
	low_draw_rec(x - 3, y - 3, x  - 3,y1 + 1,COLOR_848484,window_width,buf);
	low_draw_rec(x - 3, y1 +2, x1 + 1,y1 + 2,COLOR_FFFFFF,window_width,buf);
	low_draw_rec(x1 +2, y - 3, x1 + 2,y1 + 2,COLOR_FFFFFF,window_width,buf);
	low_draw_rec(x - 1, y - 2, x1 + 0,y - 2,COLOR_000000,window_width,buf);
	low_draw_rec(x - 2, y - 2, x -  2,y1 + 0,COLOR_000000,window_width,buf);
	low_draw_rec(x - 2, y1 +1, x1 + 0,y1 + 1,COLOR_C6C6C6,window_width,buf);
	low_draw_rec(x1 +1, y - 2, x1 + 1,y1 + 1,COLOR_C6C6C6,window_width,buf);
	low_draw_rec(x - 1, y - 1, x1 + 0,y1 + 0,color,window_width,buf);
	return 0;
}

/************************************************************************/
/* ��������                                                                     */
/************************************************************************/
int draw_up_triangle(int x,int y,u32 color,void * buf,int width){

	int i;
	draw_window_point(x + 3,y,color,buf,width);
	for (i = 0; i < 3;i++)
	draw_window_point(x + i + 2,y + 1,color,buf,width);
	for (i = 0; i < 5;i++)
	draw_window_point(x + i + 1,y + 2,color,buf,width);
	for (i = 0; i < 7;i++)
	draw_window_point(x + i,y + 3,color,buf,width);

	return 0;
}
/************************************************************************/
/* ����������                                                                     */
/************************************************************************/
int draw_down_triangle(int x,int y,u32 color,void * buf,int width){
	
	int i;
	draw_window_point(x + 3,y + 3,color,buf,width);
	for (i = 0; i < 3;i++)
		draw_window_point(x + i + 2,y + 2,color,buf,width);
	for (i = 0; i < 5;i++)
		draw_window_point(x + i + 1,y + 1,color,buf,width);
	for (i = 0; i < 7;i++)
		draw_window_point(x + i,y,color,buf,width);
	
	return 0;
}
/************************************************************************/
/* �б��                                                                     */
/************************************************************************/
int draw_listbox(int x,int y,int width,int height,int widow_width,void * buf){
	
    draw_textbox_color(x,y,width,height,widow_width,buf,COLOR_000000);
	inter_draw_rec(x + width - 16,y,x + width,y + height,COLOR_FFFFFF,COLOR_C6C6C6,widow_width,buf);
    draw_button(x + width - 15,y - 1,15,15,widow_width,buf);
	draw_button(x + width - 15,y + height / 2 - 10,15,15,widow_width,buf);
	draw_button(x + width - 15,y + height - 15,15,15,widow_width,buf);
	
	draw_up_triangle(x + width - 10,y + 5,COLOR_000000,buf,widow_width);
    draw_down_triangle(x + width - 10,y + height - 9,COLOR_000000,buf,widow_width);

	return 0;
}
//��Բ
void circle(int x,int y,int r,u32 color,void * buf,int widow_width){

	int ox,oy,d;

	if(!r)
		return;
	oy =  r;
	d = 3 - (r << 1);
	for(ox = 0;ox <= oy;ox++){
		draw_window_point(x + ox,y + oy,color,buf,widow_width);
		draw_window_point(x + ox,y - oy,color,buf,widow_width);
		draw_window_point(x - ox,y - oy,color,buf,widow_width);
		draw_window_point(x - ox,y + oy,color,buf,widow_width);
		
		draw_window_point(x + oy,y + ox,color,buf,widow_width);
		draw_window_point(x + oy,y - ox,color,buf,widow_width);
		draw_window_point(x - oy,y - ox,color,buf,widow_width);
		draw_window_point(x - oy,y + ox,color,buf,widow_width);
		if(d < 0)
			d += ox * 4 + 6;
		else 
			d +=(ox - oy--) * 4 + 10;
	}
}

u32 RGB_MIX(u8 R,u8 G,u8 B){

	u32 ret_val;
	ret_val = (u32)((B << 16) | (G << 8) | R);

	return ret_val;
}
void draw_window_bar(int x1,int y1,int x2,int y2,u32 color,int width,void * buf){

     int step, i;

	 step = 1 ;

	 for(i = 0; i < x2 - x1; i++)
	 	low_draw_rec(x1 + step * i,y1,x1 + step * (i + 1),y2,
	            color,width,buf);
}

//�����ں���
int draw_window(LPNRECT t,void * buf){
	int i,j;
	unsigned short * p;
	//�����
	low_draw_rec(0,0,3,t->height-2,COLOR_TITAL,t->width,buf);
	low_draw_rec(t->width-6,5,t->width-6,t->height-6,0xC1AB79,t->width,buf);
	low_draw_rec(4,t->height-6,t->width-6,t->height-6,0xC1AB79,t->width,buf);
	//�Ҳ���
	low_draw_rec(t->width-6,0,t->width-1,t->height-1,COLOR_TITAL,t->width,buf);
	//���ڵ�ɫ
	low_draw_rec(4,7,t->width-8,t->height-8,0xEBEBEB,t->width,buf);
	//���ڱ���
    	low_draw_rec(3,0,t->width-4,23,COLOR_TITAL,t->width,buf);
	//�����²���
	low_draw_rec(0,t->height-6,t->width-1,t->height-1,COLOR_TITAL,t->width,buf);
	
	draw_engchar(14,4,"Window",COLOR_FFFFFF,buf,t->width);
	//���ر�ͼ��	
	p = (unsigned short*)&_closebtn;

	for(j=5;j<19;j++){
		for (i=t->width-25;i<t->width-5;i++)
		{
			if (0 == *p)
			{
				draw_window_point(i,j,0xffffff,buf,t->width);
			}else{
				draw_window_point(i,j,0xC65255,buf,t->width);
			}
			p++;
		}
	}
	//���ͼ��
	
	p = (unsigned short*)&_maxbtn;
	
	for(j = 5;j < 19;j++){
		for (i=t->width-43;i<t->width-27;i++)
		{
			if (0 == *p)
			{
				draw_window_point(i,j,0,buf,t->width);
			}
			p++;
		}
	}
	//��С��ͼ��
	p = (unsigned short*)&_minbtn;

	for(j=5;j<19;j++){
		for (i=t->width-61;i<t->width-45;i++)
		{
			if (0 == *p)
			{
				draw_window_point(i,j,0,buf,t->width);
			}
			p++;
		}
	}

	return 0;
}

//ȡ�㺯��
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
	if(color_bit == 8){
		ret_val = (u32)*(p1 + p_offset);
	}
	else if(color_bit == 32){

        	ret_val = *(p3 + p_offset);
	}else{
		ret_val = (u32)*(p2 + p_offset);
	}
	return ret_val;
}
