[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[OPTIMIZE 1]
[OPTION 1]
[BITS 32]
	EXTERN	_api_putstr0
	EXTERN	_api_end
[FILE "hello4.c"]
[SECTION .data]
LC0:
	DB	"hello, world",0x0A,0x00
[SECTION .text]
	GLOBAL	_HariMain
_HariMain:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	LC0
	CALL	_api_putstr0
	POP	EAX
	LEAVE
	JMP	_api_end
