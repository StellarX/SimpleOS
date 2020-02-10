[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[OPTIMIZE 1]
[OPTION 1]
[BITS 32]
	EXTERN	_io_out8
	EXTERN	_io_in8
	EXTERN	_fifo32_put
[FILE "keyboard.c"]
[SECTION .text]
	GLOBAL	_inthandler21
_inthandler21:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	97
	PUSH	32
	CALL	_io_out8
	PUSH	96
	CALL	_io_in8
	ADD	EAX,DWORD [_keydata0]
	PUSH	EAX
	PUSH	DWORD [_keyfifo]
	CALL	_fifo32_put
	LEAVE
	RET
	GLOBAL	_wait_KBC_sendready
_wait_KBC_sendready:
	PUSH	EBP
	MOV	EBP,ESP
L3:
	PUSH	100
	CALL	_io_in8
	POP	EDX
	AND	EAX,2
	JNE	L3
	LEAVE
	RET
	GLOBAL	_init_keyboard
_init_keyboard:
	PUSH	EBP
	MOV	EBP,ESP
	MOV	EAX,DWORD [8+EBP]
	MOV	DWORD [_keyfifo],EAX
	MOV	EAX,DWORD [12+EBP]
	MOV	DWORD [_keydata0],EAX
	CALL	_wait_KBC_sendready
	PUSH	96
	PUSH	100
	CALL	_io_out8
	CALL	_wait_KBC_sendready
	POP	ECX
	POP	EAX
	MOV	DWORD [12+EBP],71
	MOV	DWORD [8+EBP],96
	LEAVE
	JMP	_io_out8
	GLOBAL	_keyfifo
[SECTION .data]
	ALIGNB	4
_keyfifo:
	RESB	4
	GLOBAL	_keydata0
[SECTION .data]
	ALIGNB	4
_keydata0:
	RESB	4
