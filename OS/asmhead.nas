; haribote-os boot asm
; TAB=4

[INSTRSET "i486p"]				; “想要使用486指令”的叙述

VBEMODE	EQU		0x105			; 1024 x  768 x 8bitカラー
; （画面モード一覧）
;	0x100 :  640 x  400 x 8bitカラー
;	0x101 :  640 x  480 x 8bitカラー
;	0x103 :  800 x  600 x 8bitカラー
;	0x105 : 1024 x  768 x 8bitカラー
;	0x107 : 1280 x 1024 x 8bitカラー

BOTPAK	EQU		0x00280000		; bootpack的装载处
DSKCAC	EQU		0x00100000		; 磁盘缓存的地方
DSKCAC0	EQU		0x00008000		; 磁盘高速缓存的场所(实时模式)

; BOOT_INFO 启动时的信息
CYLS	EQU		0x0ff0			; 设定启动区
LEDS	EQU		0x0ff1
VMODE	EQU		0x0ff2			; 这个地址用于保存关于颜色数目的信息。颜色的位数
SCRNX	EQU		0x0ff4			; 这个地址用于保存分辨率的X（screen x）
SCRNY	EQU		0x0ff6			; 这个地址用于保存分辨率的Y（screen y）
VRAM	EQU		0x0ff8			; 这个地址用于保存显存的开始地址

		ORG		0xc200			; 把程序装载到内存的哪

; VBE存在确认

		MOV		AX,0x9000
		MOV		ES,AX
		MOV		DI,0
		MOV		AX,0x4f00
		INT		0x10
		CMP		AX,0x004f
		JNE		scrn320  ;要是AX没有变为这个值，就只能使用320×200的画面

; 检查VBE的版本

		MOV		AX,[ES:DI+4]
		CMP		AX,0x0200
		JB		scrn320			; if (AX < 0x0200) goto scrn320

; 取得画面模式信息

		MOV		CX,VBEMODE
		MOV		AX,0x4f01
		INT		0x10
		CMP		AX,0x004f
		JNE		scrn320 ;如果它是0x004f以外的值，就意味着所指定的画面模式不能使用

; 画面模式信息的确认

		CMP		BYTE [ES:DI+0x19],8
		JNE		scrn320
		CMP		BYTE [ES:DI+0x1b],4
		JNE		scrn320
		MOV		AX,[ES:DI+0x00]
		AND		AX,0x0080
		JZ		scrn320			; 模式属性的bit7是0，所以放弃

; 以上条件都满足，就切换画面

		MOV		BX,VBEMODE+0x4000
		MOV		AX,0x4f02
		INT		0x10
		MOV		BYTE [VMODE],8	; 画面モードをメモする（C言語が参照する）
		MOV		AX,[ES:DI+0x12]
		MOV		[SCRNX],AX
		MOV		AX,[ES:DI+0x14]
		MOV		[SCRNY],AX
		MOV		EAX,[ES:DI+0x28]
		MOV		[VRAM],EAX
		JMP		keystatus

scrn320:
		MOV		AL,0x13			; VGAグラフィックス、320x200x8bitカラー
		MOV		AH,0x00
		INT		0x10
		MOV		BYTE [VMODE],8	; 画面モードをメモする（C言語が参照する）
		MOV		WORD [SCRNX],320
		MOV		WORD [SCRNY],200
		MOV		DWORD [VRAM],0x000a0000
		
; 用BIOS取得键盘上各种LED指示灯的状态

keystatus:
		MOV		AH,0x02
		INT		0x16 			; keyboard BIOS
		MOV		[LEDS],AL		

;   使PIC不接受一切中断
;	在AT兼容的规范中，如果要初始化PIC的话，
;	必须在CLI之前进行，否则有时会挂起
;	随后进行PIC的初始化

		MOV		AL,0xff
		OUT		0x21,AL			; 禁止主PIC的全部中断
		NOP						; 如果连续执行OUT指令，有些机种会无法正常运行
		OUT		0xa1,AL			;禁止从PIC的全部中断

		CLI						; 禁止CPU级别的中断

; 为了让CPU能够访问1MB以上的内存空间，设定A20GATE

		CALL	waitkbdout
		MOV		AL,0xd1
		OUT		0x64,AL
		CALL	waitkbdout
		MOV		AL,0xdf			; enable A20
		OUT		0x60,AL
		CALL	waitkbdout		;这句话是为了等待完成执行指令

; 切换到保护模式

		LGDT	[GDTR0]			; 设定临时GDT
		MOV		EAX,CR0
		AND		EAX,0x7fffffff	; 设bit31为0（为了禁止分页）
		OR		EAX,0x00000001	; 设bit0为1（为了切换到保护模式）
		MOV		CR0,EAX
		JMP		pipelineflush
pipelineflush:
		MOV		AX,1*8			;  可读写的段 32bit
		MOV		DS,AX
		MOV		ES,AX
		MOV		FS,AX
		MOV		GS,AX
		MOV		SS,AX

; bootpack的转送

		MOV		ESI,bootpack	; 转送源
		MOV		EDI,BOTPAK		; 转送目的地
		MOV		ECX,512*1024/4
		CALL	memcpy			;将bootpack.hrb复制到0x00280000号地址的处理

; 磁盘数据最终转送到它本来的位置去

; 首先从启动扇区开始

		MOV		ESI,0x7c00		; 转送源
		MOV		EDI,DSKCAC		; 转送目的地
		MOV		ECX,512/4		
		CALL	memcpy			;将启动扇区复制到1MB以后的内存去

; 所有剩下的

		MOV		ESI,DSKCAC0+512	; 转送源
		MOV		EDI,DSKCAC+512	; 转送目的地
		MOV		ECX,0
		MOV		CL,BYTE [CYLS]
		IMUL	ECX,512*18*2/4	; 从柱面数变换为字节数/4
		SUB		ECX,512/4		; 减去 IPL
		CALL	memcpy			;将始于0x00008200的磁盘内容，复制到0x00100200那里

; 必须由asmhead来完成的工作，至此全部完毕
;	以后就交由bootpack来完成

; bootpack的启动

		MOV		EBX,BOTPAK
		MOV		ECX,[EBX+16]
		ADD		ECX,3			; ECX += 3;
		SHR		ECX,2			; ECX /= 4;
		JZ		skip			; 没有要转送的东西时
		MOV		ESI,[EBX+20]	; 转送元
		ADD		ESI,EBX
		MOV		EDI,[EBX+12]	; 转送目的地
		CALL	memcpy
skip:
		MOV		ESP,[EBX+12]	; 栈初始值
		JMP		DWORD 2*8:0x0000001b

waitkbdout:
		IN		 AL,0x64
		AND		 AL,0x02
		IN       AL,0x60        ; 空读（为了清空数据接收缓冲区中的垃圾数据）
		JNZ		waitkbdout		;  AND的结果如果不是0，就跳到waitkbdout
		RET

memcpy:
		MOV		EAX,[ESI]
		ADD		ESI,4
		MOV		[EDI],EAX
		ADD		EDI,4
		SUB		ECX,1
		JNZ		memcpy			; 减法运算的结果如果不是0，就跳转到memcpy
		RET
; memcpyはアドレスサイズプリフィクスを入れ忘れなければ、ストリング命令でも書ける

		ALIGNB	16
GDT0:
		RESB	8				; NULL selector
		DW		0xffff,0x0000,0x9200,0x00cf	; 可以读写的段（segment）32bit
		DW		0xffff,0x0000,0x9a28,0x0047	; 可以执行的段（segment）32bit（bootpack用）

		DW		0
GDTR0:
		DW		8*3-1
		DD		GDT0

		ALIGNB	16
bootpack: