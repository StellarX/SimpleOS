项目说明：   一个32位的操作系统

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
	int.c：初始化PIC（可编程中断控制器）
	hankaku.txt: 字体数据
Makefile：包括文件生成规则和命令
make.bat：调用make.exe(这个内部的命令不懂)，而make.exe的作用是进入makefile文件，然后执行里面的命令

改动说明：
7.1
获取按键编码；
将中断处理程序中的绘制图像的程序移出，加快中断处理
将接收的按键编码存入缓冲变量

6.3
编写鼠标和键盘的中断处理程序（并注册到IDT表），具体执行过程：
	首先在naskfunc中执行用汇编写的中断现场保护的程序（asm_inthandler21），
	然后用call指令跳转到int.c中执行中断处理程序（void inthandler21(int *esp)），
	最后返回naskfunc执行恢复现场以及中断返回（IRETD）的程序
（注意：IDT表中注册的是naskfunc中汇编程序的起始地址）
最后解除鼠标、键盘的中断屏蔽位

6.2
添加int.c文件，用于初始化中断控制器PIC

6.1
添加头文件，分割编译

5.4
全局段号记录表GDT和中断记录表IDT初始化

5.3
显示变量的值和鼠标指针到屏幕

5.2
使用OSASK的字体数据hankaku.txt，将其编译为obj文件后和bootpack.obj链接；
添加绘制字符串的函数

5.1
使用结构体简化代码；
显示字符A

4.3 
绘制任务栏

4.2
处理了颜色问题（使用调色板方式）；
绘制矩形

4.1 
往显存写入数据

3.5 
添加naskfunc.nas文件，存放用汇编写的函数，编译后与bootpack.obj链接；
在bootpack里引用naskfunc.nas中的函数

3.4 
删除haribote.nas ，分解为asmhead.nas和bootpack.c；
32位模式前期准备（从BIOS得到键盘状态、保存当前画面模式到内存 etc.）；
设置显卡内存；
开始导入C语言；
makefile文件里添加了相关文件生成规则

3.3
在haribote.nas中添加设定画面模式的代码；
在ipl执行完后，OS被装载到了内存，所以JMP到haribote在内存中的位置，执行OS
ipl文件名改为ipl10（10个柱面）

3.2
读盘程序/启动程序加载器已经制作完成，开始着手开发操作系统；
添加haribote.nas文件(os目前就存放于此)，将其编译为sys文件，并保存到img文件（借助edimg.exe）