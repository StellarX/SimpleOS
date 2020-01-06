项目说明：   一个32位的操作系统

os文件夹：主要项目文件
z_tools文件夹：项目工具

os文件夹内：

ipl.nas：启动程序加载器
Makefile：包括文件生成规则和命令
make.bat：调用make.exe(这个内部的命令不懂)，而make.exe的作用是进入makefile文件，然后执行里面的命令
run.bat：运行pc模拟器

本次改动说明：
将clean、src_only命令添加到Makefile中
