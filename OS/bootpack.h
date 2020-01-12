/* asmhead.nas */
struct BOOTINFO { /* 0x0ff0-0x0fff */
	char cyls; /* 启动区读硬盘读到何处为止 */
	char leds; /* 启动时键盘LED的状态  */
	char vmode; /* 显卡模式为多少位彩色 */
	char reserve;
	short scrnx, scrny; //2B * 2
	char *vram;  //4B  当前模式的显存起始地址
};
#define ADR_BOOTINFO	0x00000ff0

/* naskfunc.nas */
void io_hlt(void);
void io_cli(void);
void io_out8(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);
void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);
void asm_inthandler21(void);
void asm_inthandler27(void);
void asm_inthandler2c(void);

/* graphic.c */
void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);//画矩形
void init_screen8(char *vram, int x, int y);//初始化屏幕，绘制任务栏
void putfont8(char *vram, int xsize, int x, int y, char c, char *font);//绘制单个字符
void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s);//显示字符串，asc表示字符编码使用ASCII
void init_mouse_cursor8(char *mouse, char bc);//初始化鼠标指针的颜色信息
void putblock8_8(char *vram, int vxsize, int pxsize,
	int pysize, int px0, int py0, char *buf, int bxsize);//把存储的颜色信息放到显存（鼠标的）
#define COL8_000000		0
#define COL8_FF0000		1
#define COL8_00FF00		2
#define COL8_FFFF00		3
#define COL8_0000FF		4
#define COL8_FF00FF		5
#define COL8_00FFFF		6
#define COL8_FFFFFF		7
#define COL8_C6C6C6		8
#define COL8_840000		9
#define COL8_008400		10
#define COL8_848400		11
#define COL8_000084		12
#define COL8_840084		13
#define COL8_008484		14
#define COL8_848484		15

/* dsctbl.c */
struct SEGMENT_DESCRIPTOR {
	short limit_low, base_low;
	char base_mid, access_right;
	char limit_high, base_high;
};
struct GATE_DESCRIPTOR {
	short offset_low, selector;
	char dw_count, access_right;
	short offset_high;
};
void init_gdtidt(void);
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);

#define ADR_IDT			0x0026f800   
#define LIMIT_IDT		0x000007ff
#define ADR_GDT			0x00270000//GDT起始地址
#define LIMIT_GDT		0x0000ffff
#define ADR_BOTPAK		0x00280000//段号为2的段，大小是512KB，地址是0x280000 为bootpack.hrb而准备。用这个段可以执行bootpack.hrb
#define LIMIT_BOTPAK	0x0007ffff
#define AR_DATA32_RW	0x4092//系统专用，可读写的段。不可执行
#define AR_CODE32_ER	0x409a//系统专用，可执行的段。可读不可写
#define AR_INTGATE32	0x008e

/* int.c */
void init_pic(void);
void inthandler21(int *esp);
void inthandler27(int *esp);
void inthandler2c(int *esp);
#define PIC0_ICW1		0x0020
#define PIC0_OCW2		0x0020
#define PIC0_IMR		0x0021
#define PIC0_ICW2		0x0021
#define PIC0_ICW3		0x0021
#define PIC0_ICW4		0x0021
#define PIC1_ICW1		0x00a0
#define PIC1_OCW2		0x00a0
#define PIC1_IMR		0x00a1
#define PIC1_ICW2		0x00a1
#define PIC1_ICW3		0x00a1
#define PIC1_ICW4		0x00a1
