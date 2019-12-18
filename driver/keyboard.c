#include "keyboard.h"
#include "../kernel/msg.h"
#include "../kernel/interrup.h"
#include "../driver/hd.h"
#include "../kernel/msg.h"
#include "../kernel/process.h"
#include "../gui/gdi.h"
#include "keymap.h"

extern KernelMessageQueue KnMsgQ;
extern int keyboard_handle;
KB_INPUT kb_in; //键盘缓存队(原始队列)
KB_INPUT secondy;
int code_with_E0,column,shift_l,shift_r,ctrl_l,ctrl_r,alt_l,alt_r;
char output_char;
void keyboard_read();


void keyboard_init(){

	 kb_in.count = 0;
	 kb_in.head = kb_in.tail = kb_in.buf;
	 
	 memset((void *)secondy.buf,0,buffer_len);
	 secondy.head = secondy.tail = secondy.buf;
	 
    	 set_interrupt((u32)&keyboard_handle,0x21);
	 outb(0x02, 0x21);
	
}

void handle_buffer(unsigned char scan_code){
	
	if(kb_in.count < buffer_len){
		*(kb_in.head) = scan_code;
		kb_in.head++;
		if(kb_in.head == kb_in.buf + buffer_len){
			kb_in.head = kb_in.buf;
			}
		kb_in.count++;	
	}
}

void send_kbmsg_to_kernel(int al){
	int makecode;
	MSG message;
	char s;

	handle_buffer((u8)al);
	keyboard_read();

	debug_print_char(output_char);
	
	makecode = al &  0x80 ? 0 : 1;
	
	if (makecode == 1)
	{
		s = (char)key_map[al * 3];
		if(s > 32 && s  < 128){
			*(secondy.head) = s;
			secondy.head++;
			if(secondy.head == secondy.buf + buffer_len){
				secondy.head = secondy.buf;
			}
		}
		message.message = WM_KEYDOWN ;
		message.lParam = (unsigned int)output_char;
	}else{
        message.message = WM_KEYUP;
	
	}
	
	sys_KernelPutMessage(&KnMsgQ,&message);
	sys_wakeup_process(task[1]);
}

u8 get_byte_from_kbuf()       /* 从键盘缓冲区中读取下一个字节 */
{
        u8 scan_code;

        while (kb_in.count <= 0) {}   /* 等待下一个字节到来 */

        scan_code = *(kb_in.tail);
        kb_in.tail++;
        if (kb_in.tail == kb_in.buf + KB_IN_BYTES) {
                kb_in.tail = kb_in.buf;
        }
        kb_in.count--;
        

        return scan_code;
}


void keyboard_read()
{
	u8	scan_code;
	char	output[2];
	int	i,make;	/* 1: make;  0: break. */
	
	u32	key = 0;/* 用一个整型来表示一个键。比如，如果 Home 被按下，
			 * 则 key 值将为定义在 keyboard.h 中的 'HOME'。
			 */
	u32*	keyrow;	/* 指向 keymap[] 的某一行 */

	if(kb_in.count > 0){
		code_with_E0 = 0;

		scan_code = get_byte_from_kbuf();

		/* 下面开始解析扫描码 */
		if (scan_code == 0xE1) {
			u8 pausebrk_scode[] = {0xE1, 0x1D, 0x45,0xE1, 0x9D, 0xC5};
			int is_pausebreak = 1;
			for(i=1;i<6;i++){
				if (get_byte_from_kbuf() != pausebrk_scode[i]) {
					is_pausebreak = 0;
					break;
				}
			}
			if (is_pausebreak) {
				key = PAUSEBREAK;
			}
		}
		else if (scan_code == 0xE0) {
			scan_code = get_byte_from_kbuf();

			/* PrintScreen 被按下 */
			if (scan_code == 0x2A) {
				if (get_byte_from_kbuf() == 0xE0) {
					if (get_byte_from_kbuf() == 0x37) {
						key = PRINTSCREEN;
						make = 1;
					}
				}
			}
			/* PrintScreen 被释放 */
			if (scan_code == 0xB7) {
				if (get_byte_from_kbuf() == 0xE0) {
					if (get_byte_from_kbuf() == 0xAA) {
						key = PRINTSCREEN;
						make = 0;
					}
				}
			}
			/* 不是PrintScreen, 此时scan_code为0xE0紧跟的那个值. */
			if (key == 0) {
				code_with_E0 = 1;
			}
		}
		if ((key != PAUSEBREAK) && (key != PRINTSCREEN)) {
			/* 首先判断Make Code 还是 Break Code */
			make = (scan_code & FLAG_BREAK ? 0 : 1);

			/* 先定位到 keymap 中的行 */
			keyrow = &key_map[(scan_code & 0x7F) * MAP_COLS];
			
			column = 0;
			if (shift_l || shift_r) {
				column = 1;
			}
			if (code_with_E0) {
				column = 2; 
				code_with_E0 = 0;
			}
			
			key = keyrow[column];
			
			switch(key) {
			case SHIFT_L:
				shift_l = make;
				break;
			case SHIFT_R:
				shift_r = make;
				break;
			case CTRL_L:
				ctrl_l = make;
				break;
			case CTRL_R:
				ctrl_r = make;
				break;
			case ALT_L:
				alt_l = make;
				break;
			case ALT_R:
				alt_l = make;
				break;
			default:
				break;
			}

			if (make) {
				key |= shift_l	? FLAG_SHIFT_L	: 0;
				key |= shift_r	? FLAG_SHIFT_R	: 0;
				key |= ctrl_l	? FLAG_CTRL_L	: 0;
				key |= ctrl_r	? FLAG_CTRL_R	: 0;
				key |= alt_l	? FLAG_ALT_L	: 0;
				key |= alt_r	? FLAG_ALT_R	: 0;
			
				if(!(key & FLAG_EXT)){
					debug_printb("deal finish\n");
					output_char = key & 0xff;
				}
			}

		}
	}
}

/*======================================================================*
			    get_byte_from_kbuf
 *======================================================================*/


