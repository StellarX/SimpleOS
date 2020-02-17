# 一、项目说明
一个32位的多任务图形操作系统（目前已实现中断处理、内存管理、图层叠加处理、定时器管理、
多任务、英文字符输入、命令行、应用程序）

# 二、运行方法
下载压缩包并解压，然后进入OS文件夹，点击!cons_nt.bat，输入make run_full，即可运行

# 三、文件说明
## z_tools文件夹
内有项目工具

## os文件夹内

##### haribote文件夹：os核心代码

- asmhead.nas: 对OS的各种设置，以及为了调用bootpack所写的程序
- ipl10.nas：启动程序加载器（将OS从磁盘读入内存
- naskfunc.nas：存放用汇编写的函数(包括API)，编译后与bootpack.obj链接
- bootpack.h：常数设定和各种函数、结构体的声明
- bootpack.c:  OS主任务
- dsctbl.c：OS有关GDT、IDT设定的源程序
- graphic.c：绘制图像的源程序
- int.c：初始化PIC（可编程中断控制器），以及其他中断处理程序
- fifo.c：循环队列缓冲区的初始化、读写数据等操作
- keyboard.c：初始化键盘控制电路以及键盘中断处理程序
- mouse.c：激活鼠标、鼠标信息解读、鼠标中断处理程序
- sheet.c:有关图层和叠加处理的程序
- memory.c：与内存管理相关的程序
- timer.c:定时器相关（含定时器中断处理程序）
- mtask.c：多任务相关
- console.c：命令行窗口任务相关（控制台）（包括各种命令的处理函数）
- window.c：窗口相关函数
- file.c：FAT解压缩、搜索磁盘文件、将文件装载到内存等函数（后两个type命令中要用）
- hankaku.txt：字体数据

##### apilib文件夹：应用程序接口（API）以及库文件
- api001~020.nas ：api
- apilib.lib：库

##### 其他文件夹：各种应用程序

##### 文件生成规则和命令
- Makefile
- make.bat：调用make.exe(这个内部的命令不懂)，而make.exe的作用是进入makefile文件，然后执行里面的命令

# 其他
### 文件编译过程图
https://www.processon.com/view/link/5bac826ee4b0bd4db94e91c1
### GDT表模型
https://www.processon.com/view/link/5bad7f8fe4b0fe81b6499b99
### 多任务控制结构示意图
https://www.processon.com/view/link/5e305525e4b00fbcc461e8a7
