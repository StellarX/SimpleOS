项目说明：   一个32位的操作系统
文件生成结构图：https://www.processon.com/diagraming/5bac826ee4b0bd4db94e91bb

z_tools文件夹：项目工具

os文件夹内：（主要项目文件）
ipl10.nas：启动程序加载器（将OS从磁盘读入内存）

os相关文件：
	asmhead.nas:  对OS的各种设置，以及为了调用bootpack所写的程序
	naskfunc.nas：存放用汇编写的函数，编译后与bootpack.obj链接
	bootpack.c:  以后为了启动OS，还要做各种设定

Makefile：包括文件生成规则和命令
make.bat：调用make.exe(这个内部的命令不懂)，而make.exe的作用是进入makefile文件，然后执行里面的命令


改动说明：
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