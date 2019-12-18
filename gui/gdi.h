#ifndef _GDI_H
#define _GDI_H

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

#define COLOR_008484   0x8484
#define COLOR_008080   0x8080
#define COLOR_C6C6C6   0xEBEBEB
#define COLOR_FFFFFF   0xFFFFFF
#define COLOR_848484   0x848484
#define COLOR_000000   0xF0C869
#define COLOR_000015   0x0451AB
#define COLOR_TITAL    0xEFC966

typedef struct _tag_RECT{
	int x,y;	
	int width,height;
}NRECT,*LPNRECT;

extern void gdi_init();
extern u32 get_point_color(int x,int y,void *buf,int width);
extern void draw_engchar(int x,int y,char * s,u32 color,u32 * buffer,int width);
extern void draw_char(int x,int y,char s,u32 color,u32 * buffer,int width);

extern int draw_window_point(int x,int y,u32 color,void * video,int width);
extern int low_draw_rec(int x1,int y1,int x2,int y2,u32 color,int width,void * buf);
extern int sys_drawtaskbar();
extern int copy_window_to_screen(int x, int y, void * buf,int width,int height);
extern int draw_textbox(int x,int y,int width,int height,int window_width,void * buf);
extern int draw_button(int x,int y,int width,int height,int window_width, void *buf);
extern int draw_listbox(int x,int y,int width,int height,int widow_width,void * buf);
extern void draw_window_bar(int x1,int y1,int x2,int y2,u32 color,int width,void * buf);
extern int draw_textbox_color(int x,int y,int width,int height,int window_width,void * buf,u32 color);
extern int draw_window(LPNRECT t,void * buf);

#endif
