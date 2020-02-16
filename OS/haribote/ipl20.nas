; hello-os
; TAB=4
	
CYLS	EQU		20				; 读取多少柱面

		ORG		0x7c00			; 指明程序的装载地址

; 以下这段是标准FAT12格式软盘专用的代码

		JMP    entry
        DB     0x90
        DB     "HARIBOTE"     ; 启动区的名称可以是任意的字符串（8字节）
        DW     512            ; 每个扇区（sector）的大小（必须为512字节）
        DB     1              ; 簇（cluster）的大小（必须为1个扇区）
        DW     1        ; FAT的起始位置（一般从第一个扇区开始）
        DB     2        ; FAT的个数（必须为2）
        DW     224      ; 根目录的大小（一般设成224项）
        DW     2880     ; 该磁盘的大小（必须是2880扇区）
        DB     0xf0     ; 磁盘的种类（必须是0xf0）
        DW     9        ; FAT的长度（必须是9扇区）
        DW     18       ; 1个磁道（track）有几个扇区（必须是18）
        DW     2        ; 磁头数（必须是2）
        DD     0        ; 不使用分区，必须是0
        DD     2880     ; 重写一次磁盘大小
        DB     0,0,0x29       ; 意义不明，固定
        DD     0xffffffff     ;（可能是）卷标号码
        DB     "HARIBOTEOS "  ; 磁盘的名称（11字节）
        DB     "FAT12   "     ; 磁盘格式名称（8字节）
        RESB   18             ; 先空出18字节
; 程序主体
entry:
		MOV		AX,0			; 初始化寄存器
		MOV		SS,AX
		MOV		SP,0x7c00
		MOV		DS,AX

;读盘
		
		MOV		AX,0x0820	
		MOV		ES,AX			
		MOV		CH,0			; 柱面0
		MOV		DH,0			; 磁头0
		MOV		CL,2			; 扇区2
readloop:
		MOV		SI,0			; 失败次数记录
retry:
		MOV		AH,0x02			; AH=0x02 : 读盘
		MOV		AL,1			; 1个扇区
		MOV		BX,0
		MOV		DL,0x00			; A驱动器
		INT		0x13			; 调用磁盘BIOS
		JNC		next			; 没有carry就跳转，程序正确
		ADD		SI,1			; SI+1
		CMP		SI,5			; SI与5比较
		JAE		error			; SI >= 5 则跳转到error
		MOV		AH,0x00
		MOV		DL,0x00			; A驱动器
		INT		0x13			; 重置驱动器，系统复位
		JMP		retry
next:
		MOV		AX,ES			; 内存地址后移0x200，因为一个扇区是512B
		ADD		AX,0x0020
		MOV		ES,AX			; 因为没有ADD ES,0x020指令，所以这里稍微绕个弯
		ADD		CL,1			 
		CMP		CL,18			
		JBE		readloop		; CL <= 18 则继续读盘
		MOV		CL,1			; 一个柱面的一面读完，开始从另一面的第一扇区读
		ADD		DH,1			; 变换磁头
		CMP		DH,2
		JB		readloop		; DH < 2 则继续读盘
		MOV		DH,0
		ADD		CH,1			; 柱面+1
		CMP		CH,CYLS
		JB		readloop		; CH < CYLS 则继续读盘
		
;虽然读完了，但是因为暂时没有要做的事所以休眠
; 読み終わったのでharibote.sysを実行だ！
		MOV		[0x0ff0],CH		; IPLがどこまで読んだのかをメモ
		JMP		0xc200
		
error:
		MOV		SI,msg
putloop:
		MOV		AL,[SI]
		ADD		SI,1			; SI+1
		CMP		AL,0
		JE		fin
		MOV		AH,0x0e			; 显示一个文字的相关设定
		MOV		BX,15			; 指定字符颜色
        INT		0x10			; 调用显卡BIOS
		JMP		putloop
fin:
		HLT						; 让cpu停止，等待指令
		JMP		fin				; 无限循环
		
; 信息显示部分
msg:
		DB		0x0a, 0x0a		; 2个换行
		DB		"error"
		DB		0x0a			; 改行
		DB		0

		RESB	0x7dfe-$		; 0x7dfeまでを0x00で埋める命令  填写0x00,直到 0x007dfe

		DB		0x55, 0xaa