项目说明：   一个32位的图形操作系统（目前已实现中断处理、内存管理、图层叠加处理、定时器管理、
多任务、英文字符输入）

运行方法：下载压缩包并解压，然后进入OS文件夹，点击!cons_nt.bat，输入make run，即可运行

z_tools文件夹：项目工具
os文件夹内：项目源代码
ipl10.nas：启动程序加载器（将OS从磁盘读入内存）
os源代码：
	asmhead.nas:  对OS的各种设置，以及为了调用bootpack所写的程序
	naskfunc.nas：存放用汇编写的函数，编译后与bootpack.obj链接
	bootpack.h：常数设定和各种函数、结构体的声明
	bootpack.c:  OS主函数
	dsctbl.c：OS有关GDT、IDT设定的源程序
	graphic.c：绘制图像的源程序
	int.c：初始化PIC（可编程中断控制器），以及其他中断处理程序
	fifo.c：循环队列缓冲区的初始化、读写数据等操作
	keyboard.c：初始化键盘控制电路以及键盘中断处理程序
	mouse.c：激活鼠标、鼠标信息解读、鼠标中断处理程序
	sheet.c:有关图层和叠加处理的程序
	memory.c：与内存管理相关的程序
	timer.c:定时器相关（含定时器中断处理程序）
	mtask.c：多任务相关
	hankaku.txt: 字体数据
Makefile：包括文件生成规则和命令
make.bat：调用make.exe(这个内部的命令不懂)，而make.exe的作用是进入makefile文件，然后执行里面的命令

其他：
文件编译过程图：https://www.processon.com/view/link/5bac826ee4b0bd4db94e91c1
GDT表模型：https://www.processon.com/view/link/5bad7f8fe4b0fe81b6499b99
