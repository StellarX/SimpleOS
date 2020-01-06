项目说明：   一个32位的操作系统

os文件夹：主要项目文件
z_tools文件夹：项目工具

os文件夹内：

ipl.nas：启动程序加载器
Makefile：用ipl.nas生成ipl.bin和ipl.lst,  根据ipl.bin制作映像文件
make.bat：调用make.exe(？？？)
run.bat：运行pc模拟器

本次改动说明：
新建Makefile代替了asm.bat和makeimg.bat
删除了asm.bat和makeimg.bat