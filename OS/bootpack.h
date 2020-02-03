/* asmhead.nas */
struct BOOTINFO { /* 0x0ff0-0x0fff */
	char cyls; /* 启动区读硬盘读到何处为止 */
	char leds; /* 启动时键盘LED的状态  1111 1111 */ 
	char vmode; /* 显卡模式为多少位彩色 */
	char reserve;
	short scrnx, scrny; //2B * 2
	char *vram;  //4B  当前模式的显存起始地址
};
#define ADR_BOOTINFO	0x00000ff0
#define ADR_DISKIMG     0x00100000

/* naskfunc.nas */
void io_hlt(void);
void io_cli(void);
void io_sti(void);
void io_stihlt(void);
int io_in8(int port);
void io_out8(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);
void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);
int load_cr0(void); 
void store_cr0(int cr0);
void load_tr(int tr);
void asm_inthandler20(void);
void asm_inthandler21(void);
void asm_inthandler27(void);
void asm_inthandler2c(void);
void asm_inthandler0d(void);
void asm_inthandler0c(void);
unsigned int memtest_sub(unsigned int start, unsigned int end);
void farjmp(int eip, int cs);
void farcall(int eip, int cs);
void asm_hrb_api(void);
void start_app(int eip, int cs, int esp, int ds, int *tss_esp0);
void asm_end_app(void);

/* fifo.c */
struct FIFO32 {
	int *buf;
	int p, q, size, free, flags;//p：下一个数据写入地址，q：下一个数据读出地址  flag：记录缓冲区是否溢出
	struct TASK *task;
};//free：缓冲区里没有数据的字节数  size：缓冲区的总字节数
void fifo32_init(struct FIFO32 *fifo, int size, int *buf, struct TASK *task);
int fifo32_put(struct FIFO32 *fifo, int data);
int fifo32_get(struct FIFO32 *fifo);
int fifo32_status(struct FIFO32 *fifo);

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
#define AR_TSS32		0x0089
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

/* keyboard.c */
void inthandler21(int *esp);
void wait_KBC_sendready(void);
void init_keyboard(struct FIFO32 *fifo, int data0);
#define PORT_KEYDAT		0x0060//从编号为0x0060的设备（键盘）输入的8位信息是按键编码
#define PORT_KEYCMD		0x0064

/* mouse.c */
struct MOUSE_DEC {
	unsigned char buf[3], phase;////phase用来记住接收鼠标数据的工作进展到了什么阶段
	int x, y, btn;//存放移动信息和鼠标按键状态
};
void inthandler2c(int *esp);
void enable_mouse(struct FIFO32 *fifo, int data0, struct MOUSE_DEC *mdec);
int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat);

/* memory.c */
#define MEMMAN_FREES		4090	
#define MEMMAN_ADDR			0x003c0000
struct FREEINFO {	/* 可用信息 */
	unsigned int addr, size;
};
struct MEMMAN {		/* 内存管理 */
	int frees, maxfrees, lostsize, losts;
	struct FREEINFO free[MEMMAN_FREES];/* 約32KB */
};

unsigned int memtest(unsigned int start, unsigned int end); //确认CPU是386还是486以上的并进行内存检查
void memman_init(struct MEMMAN *man);
unsigned int memman_total(struct MEMMAN *man);
unsigned int memman_alloc(struct MEMMAN *man, unsigned int size);
int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size);
unsigned int memman_alloc_4k(struct MEMMAN *man, unsigned int size);
int memman_free_4k(struct MEMMAN *man, unsigned int addr, unsigned int size);


/* sheet.c */
#define MAX_SHEETS		256
struct SHEET {
	unsigned char *buf;//图层首地址
	int bxsize, bysize, vx0, vy0, col_inv, height, flags;//bxsize，bysize：整体大小
	struct SHTCTL *ctl;
	struct TASK *task;
};                      //vx，vy：在画面上的坐标   col_inv 透明色色号    
struct SHTCTL {
	unsigned char *vram, *map;//VRAM的地址
	int xsize, ysize, top;//画面的大小  top代表最上面图层的高度
	struct SHEET *sheets[MAX_SHEETS];//用于存放我们准备的256个图层的结构体的地址
	struct SHEET sheets0[MAX_SHEETS];//用于存放我们准备的256个图层的信息
};
struct SHTCTL *shtctl_init(struct MEMMAN *memman, unsigned char *vram, int xsize, int ysize);
struct SHEET *sheet_alloc(struct SHTCTL *ctl);
void sheet_setbuf(struct SHEET *sht, unsigned char *buf, int xsize, int ysize, int col_inv);
void sheet_updown(struct SHEET *sht, int height);
void sheet_refresh(struct SHEET *sht, int bx0, int by0, int bx1, int by1);
void sheet_slide(struct SHEET *sht, int vx0, int vy0);
void sheet_free(struct SHEET *sht);

/* timer.c */
#define MAX_TIMER		500
struct TIMER {
	struct TIMER *next;//下一个定时器的地址
	unsigned int timeout, flags;
	struct FIFO32 *fifo;
	int data;
};
struct TIMERCTL {
	unsigned int count, next;//next 下一个超时时刻
	struct TIMER *t0;
	struct TIMER timer0[MAX_TIMER];
};
extern struct TIMERCTL timerctl;
void init_pit(void);
struct TIMER *timer_alloc(void);
void timer_free(struct TIMER *timer);
void timer_init(struct TIMER *timer, struct FIFO32 *fifo, int data);
void timer_settime(struct TIMER *timer, unsigned int timeout);
void inthandler20(int *esp);

/* mtask.c */
#define MAX_TASKS		1000	/* 最大任务数量 */
#define MAX_TASKS_LV    100		/*每个LEVEL我们设定最多允许创建100个任务*/
#define MAX_TASKLEVELS  10		/*总共10个LEVEL*/
#define TASK_GDT0		3		/* 定义从GDT的几号开始分配给TSS */
struct TSS32 {
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;//与任务设置相关的信息，在任务切换的时候不会被写入（backlink除外，某些情况下是会被写入的
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;//32位寄存器
	int es, cs, ss, ds, fs, gs;//16位寄存器
	int ldtr, iomap;//有关任务设置的部分，任务切换时不会被写入内存
};//TSS共包含26个int成员，总计104字节（摘自CPU的技术资料）
struct TASK {
	int sel, flags; /* sel用来存放GDT的编号  flag： 1休眠 2活动中*/
	int priority, level;
	struct FIFO32 fifo;
	struct TSS32 tss;
};
struct TASKLEVEL {
    int running; /*正在运行的任务数量*/
    int now; /*这个变量用来记录当前正在运行的是哪个任务*/
    struct TASK *tasks[MAX_TASKS_LV];
};
struct TASKCTL {
	int now_lv; /*现在活动中的LEVEL */
    char lv_change; /*在下次任务切换时是否需要改变LEVEL */
	struct TASKLEVEL level[MAX_TASKLEVELS];
	struct TASK tasks0[MAX_TASKS];
};
extern struct TIMER *task_timer;
struct TASK *task_now(void);//返回现在活动中的struct TASK的内存地址
struct TASK *task_init(struct MEMMAN *memman);
struct TASK *task_alloc(void);
void task_run(struct TASK *task, int level, int priority);
void task_switch(void);
void task_sleep(struct TASK *task);

void task_idle(void);//闲置的任务
void task_switchsub(void);//在任务切换时决定接下来切换到哪个LEVEL
void task_remove(struct TASK *task);//从struct TASKLEVEL中删除一个任务
void task_add(struct TASK *task);//向struct TASKLEVEL中添加一个任务


/* window.c */
void change_wtitle8(struct SHEET *sht, char act);
void make_window8(unsigned char *buf, int xsize, int ysize, char *title, char act);
void putfonts8_asc_sht(struct SHEET *sht, int x, int y, int c, int b, char *s, int l);
void make_textbox8(struct SHEET *sht, int x0, int y0, int sx, int sy, int c);
void make_wtitle8(unsigned char *buf, int xsize, char *title, char act);

/* console.c */
struct CONSOLE {
	struct SHEET *sht;
	int cur_x, cur_y, cur_c;
	struct TIMER *timer;
};
void console_task(struct SHEET *sheet, unsigned int memtotal);
void cons_putchar(struct CONSOLE *cons, int chr, char move);
void cons_putstr0(struct CONSOLE *cons, char *s);
void cons_putstr1(struct CONSOLE *cons, char *s, int l);
void cons_newline(struct CONSOLE *cons);
void cons_runcmd(char *cmdline, struct CONSOLE *cons, int *fat, unsigned int memtotal);
void cmd_mem(struct CONSOLE *cons, unsigned int memtotal);
void cmd_cls(struct CONSOLE *cons);
void cmd_dir(struct CONSOLE *cons);
void cmd_type(struct CONSOLE *cons, int *fat, char *cmdline);
int cmd_app(struct CONSOLE *cons, int *fat, char *cmdline);
int *hrb_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax);
int *inthandler0d(int *esp);//一般性保护异常中断处理
int *inthandler0c(int *esp);//栈异常
void hrb_api_linewin(struct SHEET *sht, int x0, int y0, int x1, int y1, int col);

/* file.c */
struct FILEINFO {
	unsigned char name[8], ext[3], type;
	char reserve[10];
	unsigned short time, date, clustno;//clustno代表文件从磁盘上的哪个扇区开始存放
	unsigned int size;
};
void file_readfat(int *fat, unsigned char *img);
void file_loadfile(int clustno, int size, char *buf, int *fat, char *img);
struct FILEINFO *file_search(char *name, struct FILEINFO *finfo, int max);

