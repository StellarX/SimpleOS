全局段表：
GDT：global （segment）descriptor table
TSS：task status segment
IDT：interrupt decriptor table

内存管理结构：
#define MEMMAN_FREES		4090	
#define MEMMAN_ADDR		0x003c0000
struct FREEINFO {	/* 可用信息 */
	unsigned int addr, size;
};
struct MEMMAN {		/* 内存管理 */
	int frees, maxfrees, lostsize, losts;
	struct FREEINFO free[MEMMAN_FREES];/* 約32KB */ 
};


图层控制结构：
struct SHEET {
	unsigned char *buf;//图层首地址
	int bxsize, bysize, vx0, vy0, col_inv, height, flags;//bxsize，bysize：整体大小
	struct SHTCTL *ctl;
};                      //vx，vy：在画面上的坐标   col_inv 透明色色号    
struct SHTCTL {
	unsigned char *vram, *map;//VRAM的地址
	int xsize, ysize, top;//画面的大小  top代表最上面图层的高度
	struct SHEET *sheets[MAX_SHEETS];//用于存放我们准备的256个图层的结构体的地址
	struct SHEET sheets0[MAX_SHEETS];//用于存放我们准备的256个图层的信息
};


中断控制结构：
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


多任务控制结构：
#define MAX_TASKS		1000	/* 最大任务数量 */
#define MAX_TASKS_LV    100		/*每个LEVEL我们设定最多允许创建100个任务*/
#define MAX_TASKLEVELS  10		/*总共10个LEVEL*/
#define TASK_GDT0		3	/* 定义从GDT的几号开始分配给TSS */
struct TSS32 {
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;//与任务设置相关的信息，在任务切换的时候不会被写入（backlink除外，某些情况下是会被写入的
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;//32位寄存器
	int es, cs, ss, ds, fs, gs;//16位寄存器
	int ldtr, iomap;//有关任务设置的部分，任务切换时不会被写入内存
};//TSS共包含26个int成员，总计104字节（摘自CPU的技术资料）
struct TASK {
	int sel, flags; /* sel用来存放GDT的编号 */
	int priority;
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


全局缓冲区：
struct FIFO32 {
	int *buf;
	int p, q, size, free, flags;//p：下一个数据写入地址，q：下一个数据读出地址  flag：记录缓冲区是否溢出
};//free：缓冲区里没有数据的字节数  size：缓冲区的总字节数


启动信息：
struct BOOTINFO { /* 0x0ff0-0x0fff */
	char cyls; /* 启动区读硬盘读到何处为止 */
	char leds; /* 启动时键盘LED的状态  */
	char vmode; /* 显卡模式为多少位彩色 */
	char reserve;
	short scrnx, scrny; //2B * 2
	char *vram;  //4B  当前模式的显存起始地址
};