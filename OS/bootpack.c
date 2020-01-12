#include <stdio.h>
#include "bootpack.h"

void HariMain(void)
{
	struct BOOTINFO *binfo = (struct BOOTINFO *) 0x0ff0;
	char s[40], mcursor[256];
	int mx, my;	
	
	init_gdtidt();
	init_pic();
	io_sti();//IDT/PIC的初始化结束了，设置中断标志位=1，允许中断
	
	init_palette();
	init_screen8(binfo->vram, binfo->scrnx, binfo->scrny);
	mx = (binfo->scrnx - 16) / 2; /* 坐标计算在屏幕中央*/
	my = (binfo->scrny - 28 - 16) / 2;
	init_mouse_cursor8(mcursor, COL8_008484);
	putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);
	sprintf(s, "(%d, %d)", mx, my);
	putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s);

	io_out8(PIC0_IMR, 0xf9); /* 解除中断屏蔽，允许使用PIC1和键盘(11111001)*/
	io_out8(PIC1_IMR, 0xef); /* 允许鼠标(11101111) */
	
	for (;;) {
		io_hlt();
	}
}



