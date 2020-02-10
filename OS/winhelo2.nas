[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[OPTIMIZE 1]
[OPTION 1]
[BITS 32]
	EXTERN	_api_openwin
	EXTERN	_api_boxfilwin
	EXTERN	_api_putstrwin
	EXTERN	_api_end
[FILE "winhelo2.c"]
[SECTION .data]
LC0:
	DB	"hello",0x00
LC1:
	DB	"hello, world",0x00
[SECTION .text]
	GLOBAL	_HariMain
_HariMain:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EBX
	PUSH	LC0
	PUSH	-1
	PUSH	50
	PUSH	150
	PUSH	_buf
	CALL	_api_openwin
	PUSH	3
	PUSH	43
	MOV	EBX,EAX
	PUSH	141
	PUSH	36
	PUSH	8
	PUSH	EAX
	CALL	_api_boxfilwin
	ADD	ESP,44
	PUSH	LC1
	PUSH	12
	PUSH	0
	PUSH	28
	PUSH	28
	PUSH	EBX
	CALL	_api_putstrwin
	MOV	EBX,DWORD [-4+EBP]
	ADD	ESP,24
	LEAVE
	JMP	_api_end
	GLOBAL	_buf
[SECTION .data]
	ALIGNB	16
_buf:
	RESB	7500
