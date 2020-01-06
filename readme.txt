项目说明：   一个32位的操作系统

os文件夹：主要项目文件
z_tools文件夹：项目工具

os文件夹内：

ipl.nas：启动程序加载器
Makefile：用ipl.nas生成ipl.bin和ipl.lst,  根据ipl.bin制作映像文件
make.bat：调用make.exe(？？？)
run.bat：运行pc模拟器

本次改动说明：
将批处理命令添加到Makefile中，简化命令

在Makefile中添加img、asm、run、install命令
删除run.bat