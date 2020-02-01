[FORMAT "WCOFF"]				; 这里的api_putchar需要与a.c的编译结果进行连接，因此我们使用对象文件模式
[INSTRSET "i486p"]				; 表示使用486兼容指令集
[BITS 32]						; 生成32位模式机器语言
[FILE "a_nask.nas"]				; 源文件名信息

		GLOBAL	_api_putchar

[SECTION .text]

_api_putchar:	; void api_putchar(int c);
		MOV		EDX,1
		MOV		AL,[ESP+4]		; c
		INT		0x40
		RET
