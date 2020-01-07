#$表示路径
TOOLPATH = ../z_tools/
MAKE     = $(TOOLPATH)make.exe -r
NASK     = $(TOOLPATH)nask.exe
EDIMG    = $(TOOLPATH)edimg.exe
IMGTOL   = $(TOOLPATH)imgtol.com
COPY     = copy
DEL      = del

#默认动作
default:
	$(MAKE) img


#文件生成规则

ipl.bin : ipl.nas Makefile
	$(NASK) ipl.nas ipl.bin ipl.lst

haribote.img : ipl.bin Makefile
	$(EDIMG)   imgin:../z_tools/fdimg0at.tek \
		wbinimg src:ipl.bin len:512 from:0 to:0   imgout:haribote.img
		
#命令 在cmd窗口输入，就会执行对应的命令
img:
	$(MAKE) haribote.img
asm:
	$(MAKE) ipl.bin
	
run :
	$(MAKE) img
	$(COPY) haribote.img ..\z_tools\qemu\fdimage0.bin
	$(MAKE) -C ../z_tools/qemu

install :
	$(MAKE) img
	$(IMGTOL) w a: haribote.img

clean :
	-$(DEL) ipl.bin
	-$(DEL) ipl.lst

#从下面可以看出，make.exe的作用是进入makefile文件，然后执行里面的命令
src_only:
	$(MAKE) clean
	-$(DEL) haribote.img
