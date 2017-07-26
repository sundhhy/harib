TOOLPATH = ../z_tools/
INCPATH  = ../z_tools/haribote/

OBJS_BOOTPACK = bootpack.obj naskfunc.obj hankaku.obj graphic.obj dsctbl.obj \
		int.obj fifo.obj keyboard.obj memman.obj sheet.obj timer.obj mouse.obj mtask.obj

MAKE     = $(TOOLPATH)make.exe -r
MAKEFONT = $(TOOLPATH)makefont.exe
BIN2OBJ  = $(TOOLPATH)bin2obj.exe
NASK     = $(TOOLPATH)nask.exe
CC1      = $(TOOLPATH)cc1.exe -I$(INCPATH) -Os -Wall -quiet
GAS2NASK = $(TOOLPATH)gas2nask.exe -a
OBJ2BIM  = $(TOOLPATH)obj2bim.exe
BIM2HRB  = $(TOOLPATH)bim2hrb.exe
RULEFILE = $(TOOLPATH)haribote/haribote.rul
EDIMG    = $(TOOLPATH)edimg.exe
IMGTOL   = $(TOOLPATH)imgtol.com
COPY     = cp
DEL      = rm

# デフォルト動作

default :
	$(MAKE) img

# ファイル生成規則

ipl10.bin : ipl10.nas Makefile
	$(NASK) ipl10.nas ipl10.bin ipl10.lst

asmhead.bin : asmhead.nas Makefile
	$(NASK) asmhead.nas asmhead.bin asmhead.lst

hankaku.bin : hankaku.txt Makefile
	$(MAKEFONT) hankaku.txt hankaku.bin

hankaku.obj : hankaku.bin Makefile
	$(BIN2OBJ) hankaku.bin hankaku.obj _hankaku
	
#bootpack.gas : bootpack.c Makefile
#	$(CC1) -o bootpack.gas bootpack.c

#graphic.gas : graphic.c Makefile
#	$(CC1) -o graphic.gas graphic.c
	
#dsctbl.gas : dsctbl.c Makefile
#	$(CC1) -o dsctbl.gas dsctbl.c
	
#bootpack.nas : bootpack.gas Makefile
#	$(GAS2NASK) bootpack.gas bootpack.nas
	
#graphic.nas : graphic.gas Makefile
#	$(GAS2NASK) graphic.gas graphic.nas
#dsctbl.nas : dsctbl.gas Makefile
#	$(GAS2NASK) dsctbl.gas dsctbl.nas
	


#bootpack.obj : bootpack.nas Makefile
#	$(NASK) bootpack.nas bootpack.obj bootpack.lst
	
#graphic.obj : graphic.nas Makefile
#	$(NASK) graphic.nas graphic.obj graphic.lst	
	
#dsctbl.obj : dsctbl.nas Makefile
#	$(NASK) dsctbl.nas dsctbl.obj dsctbl.lst
	
#int.obj : int.nas Makefile
#	$(NASK) int.nas int.obj int.lst

%.gas : %.c Makefile
	$(CC1) -o $*.gas $*.c
	
%.nas : %.gas Makefile
	$(GAS2NASK) $*.gas $*.nas
	
%.obj : %.nas Makefile
	$(NASK) $*.nas $*.obj $*.lst

naskfunc.obj : naskfunc.nas Makefile
	$(NASK) naskfunc.nas naskfunc.obj naskfunc.lst

bootpack.bim : $(OBJS_BOOTPACK) Makefile
	$(OBJ2BIM) @$(RULEFILE) out:bootpack.bim stack:3136k map:bootpack.map \
		$(OBJS_BOOTPACK)
# 3MB+64KB=3136KB

bootpack.hrb : bootpack.bim Makefile
	$(BIM2HRB) bootpack.bim bootpack.hrb 0
	


haribote.sys : asmhead.bin  bootpack.hrb Makefile
	haribote.bat

haribote.img : ipl10.bin haribote.sys Makefile
	$(EDIMG)   imgin:../z_tools/fdimg0at.tek \
		wbinimg src:ipl10.bin len:512 from:0 to:0 \
		copy from:haribote.sys to:@: \
		imgout:haribote.img

# コマンド

img :
	$(MAKE) haribote.img

run :
	$(MAKE) img
	$(COPY) haribote.img ../z_tools/qemu/fdimage0.bin
	$(MAKE) -C ../z_tools/qemu

install :
	$(MAKE) img
	$(IMGTOL) w a: haribote.img

clean :
	-$(DEL) *.bin
	-$(DEL) *.lst
	-$(DEL) *.gas
	-$(DEL) *.obj
	-$(DEL) bootpack.nas
	-$(DEL) bootpack.map
	-$(DEL) bootpack.bim
	-$(DEL) bootpack.hrb
	-$(DEL) haribote.sys
	
	


src_only :
	$(MAKE) clean
	-$(DEL) haribote.img
