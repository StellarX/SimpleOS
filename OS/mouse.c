/* 鼠标相关*/

#include "bootpack.h"


struct FIFO8 mousefifo;

void inthandler2c(int *esp)
/* PS/2来自鼠标的中断 */
{
	unsigned char data;
	io_out8(PIC1_OCW2, 0x64);	/*通知PIC1 IRQ-12的受理已经完成 */
	io_out8(PIC0_OCW2, 0x62);	/*通知PIC0 IRQ-02的受理已经完成*/
	data = io_in8(PORT_KEYDAT);
	fifo8_put(&mousefifo, data);
	return;
}


#define KEYCMD_SENDTO_MOUSE		0xd4
#define MOUSECMD_ENABLE			0xf4

void enable_mouse(struct MOUSE_DEC *mdec)
{
	/* 激活鼠标 */
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);// 顺利的话，ACK(0xfa)会被送过来
	mdec->phase = 0;//首先进入到等待鼠标ACK的0xfa的状态
	return; /* 顺利的话,键盘控制其会返送回ACK(0xfa) */
}


int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat)//鼠标信息的解读
{
	if (mdec->phase == 0) {
		/* 等待鼠标ACK的0xfa的状态 */
		if (dat == 0xfa) {
			mdec->phase = 1;
		}
		return 0;
	}
	if (mdec->phase == 1) {
		/* 等待鼠标的第一字节 */
		if ((dat & 0xc8) == 0x08) {
			/*正确的第一个字节 */
			mdec->buf[0] = dat;
			mdec->phase = 2;
		}
		return 0;
	}
	if (mdec->phase == 2) {
		/* 等待鼠标的第二字节 */
		mdec->buf[1] = dat;
		mdec->phase = 3;
		return 0;
	}
	if (mdec->phase == 3) {
		/* 等待鼠标的第三字节 */
		mdec->buf[2] = dat;
		mdec->phase = 1;//3个字节凑齐后，mouse_decode函数执行“return 1;”，把这些数据显示出来
		mdec->btn = mdec->buf[0] & 0x07;
		mdec->x = mdec->buf[1];
		mdec->y = mdec->buf[2];
		if ((mdec->buf[0] & 0x10) != 0) {
			mdec->x |= 0xffffff00;//x是四个字节，高3字节全设为1
		}
		if ((mdec->buf[0] & 0x20) != 0) {
			mdec->y |= 0xffffff00;
		}
		mdec->y = - mdec->y; /* 在鼠标上，y方向的符号与屏幕相反 */
		return 1;
	}
	return -1; /* 应该不会来这里*/
}


