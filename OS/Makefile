#默认动作
default:
	../z_tools/make.exe img


#文件生成规则

ipl.bin : ipl.nas Makefile
	../z_tools/nask.exe ipl.nas ipl.bin ipl.lst

helloos.img : ipl.bin Makefile
	../z_tools/edimg.exe   imgin:../z_tools/fdimg0at.tek \
		wbinimg src:ipl.bin len:512 from:0 to:0   imgout:helloos.img
		
#命令 在cmd窗口输入，就会执行对应的命令
img:
	../z_tools/make.exe -r helloos.img
asm:
	../z_tools/make.exe -r ipl.bin
	
run:
	../z_tools/make.exe img
	copy helloos.img ..\z_tools\qemu\fdimage0.bin
	../z_tools/make.exe -C ../z_tools/qemu
install :
	../z_tools/make.exe img
	../z_tools/imgtol.com w a: helloos.img
clean:
	-del ipl.bin
	-del ipl.lst

#从下面可以看出，make.exe的作用是进入makefile文件，然后执行里面的命令
src_only:
	../z_tools/make.exe clean
	-del helloos.img
