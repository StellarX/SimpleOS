; naskfunc
; TAB=4

[FORMAT "WCOFF"]				; 制作目?文件的模式	
[INSTRSET "i486p"]				; 使用486命令
[BITS 32]						; 制作32位模式用的机器?言



; 制作目?文件的信息

[FILE "naskfunc.nas"]			; 源文件名信息

		GLOBAL	_io_hlt,_write_mem8	; 程序中包含的函数名


; 以下是的??函数

[SECTION .text]		; 目?文件中写了?些之后再写程序

_io_hlt:	; void io_hlt(void);
		HLT
		RET
		
_write_mem8:	; void write_mem8(int addr, int data);
		MOV		ECX,[ESP+4]		; [ESP + 4]中存放的是地址，将其?入ECX
		MOV		AL,[ESP+8]		; [ESP + 8]中存放的是数据，将其?入AL
		MOV		[ECX],AL
		RET