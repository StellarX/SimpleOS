[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[OPTIMIZE 1]
[OPTION 1]
[BITS 32]
	EXTERN	_io_out8
	EXTERN	_io_load_eflags
	EXTERN	_io_cli
	EXTERN	_io_store_eflags
	EXTERN	_task_timer
	EXTERN	_fifo32_put
	EXTERN	_task_switch
[FILE "timer.c"]
[SECTION .text]
	GLOBAL	_init_pit
_init_pit:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	52
	PUSH	67
	CALL	_io_out8
	PUSH	156
	PUSH	64
	CALL	_io_out8
	PUSH	46
	PUSH	64
	CALL	_io_out8
	MOV	EDX,_timerctl+10000
	MOV	DWORD [_timerctl],0
	ADD	ESP,24
	MOV	EAX,_timerctl+20
L6:
	MOV	BYTE [EAX],0
	ADD	EAX,20
	CMP	EAX,EDX
	JLE	L6
	CALL	_timer_alloc
	MOV	BYTE [8+EAX],2
	MOV	DWORD [4+EAX],-1
	MOV	DWORD [EAX],0
	MOV	DWORD [_timerctl+8],EAX
	MOV	DWORD [_timerctl+4],-1
	LEAVE
	RET
	GLOBAL	_timer_alloc
_timer_alloc:
	PUSH	EBP
	XOR	EDX,EDX
	MOV	EBP,ESP
	MOV	EAX,_timerctl+12
L15:
	CMP	BYTE [8+EAX],0
	JE	L18
	INC	EDX
	ADD	EAX,20
	CMP	EDX,499
	JLE	L15
	XOR	EAX,EAX
L9:
	POP	EBP
	RET
L18:
	MOV	BYTE [8+EAX],1
	MOV	BYTE [9+EAX],0
	JMP	L9
	GLOBAL	_timer_free
_timer_free:
	PUSH	EBP
	MOV	EBP,ESP
	MOV	EAX,DWORD [8+EBP]
	MOV	BYTE [8+EAX],0
	POP	EBP
	RET
	GLOBAL	_timer_init
_timer_init:
	PUSH	EBP
	MOV	EBP,ESP
	MOV	EAX,DWORD [8+EBP]
	MOV	EDX,DWORD [12+EBP]
	MOV	DWORD [12+EAX],EDX
	MOV	EDX,DWORD [16+EBP]
	MOV	DWORD [16+EAX],EDX
	POP	EBP
	RET
	GLOBAL	_timer_settime
_timer_settime:
	PUSH	EBP
	MOV	EAX,DWORD [_timerctl]
	MOV	EBP,ESP
	PUSH	ESI
	PUSH	EBX
	MOV	EBX,DWORD [8+EBP]
	ADD	EAX,DWORD [12+EBP]
	MOV	BYTE [8+EBX],2
	MOV	DWORD [4+EBX],EAX
	CALL	_io_load_eflags
	MOV	ESI,EAX
	CALL	_io_cli
	MOV	EAX,DWORD [_timerctl+8]
	MOV	EDX,DWORD [4+EBX]
	CMP	EDX,DWORD [4+EAX]
	JBE	L28
L23:
	MOV	ECX,EAX
	MOV	EAX,DWORD [EAX]
	CMP	EDX,DWORD [4+EAX]
	JA	L23
	MOV	DWORD [ECX],EBX
	MOV	DWORD [EBX],EAX
L27:
	MOV	DWORD [8+EBP],ESI
	POP	EBX
	POP	ESI
	POP	EBP
	JMP	_io_store_eflags
L28:
	MOV	DWORD [_timerctl+8],EBX
	MOV	DWORD [_timerctl+4],EDX
	MOV	DWORD [EBX],EAX
	JMP	L27
	GLOBAL	_timer_cancel
_timer_cancel:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	ESI
	PUSH	EBX
	MOV	EBX,DWORD [8+EBP]
	CALL	_io_load_eflags
	MOV	ESI,EAX
	CALL	_io_cli
	CMP	BYTE [8+EBX],2
	JE	L37
	PUSH	ESI
	CALL	_io_store_eflags
	XOR	EAX,EAX
L29:
	LEA	ESP,DWORD [-8+EBP]
	POP	EBX
	POP	ESI
	POP	EBP
	RET
L37:
	MOV	EAX,DWORD [_timerctl+8]
	CMP	EBX,EAX
	JE	L38
L31:
	MOV	EDX,EAX
	MOV	EAX,DWORD [EAX]
	CMP	EAX,EBX
	JNE	L31
	MOV	EAX,DWORD [EBX]
	MOV	DWORD [EDX],EAX
L32:
	MOV	BYTE [8+EBX],1
	PUSH	ESI
	CALL	_io_store_eflags
	MOV	EAX,1
	JMP	L29
L38:
	MOV	EDX,DWORD [EBX]
	MOV	DWORD [_timerctl+8],EDX
	MOV	EAX,DWORD [4+EDX]
	MOV	DWORD [_timerctl+4],EAX
	JMP	L32
	GLOBAL	_timer_cancelall
_timer_cancelall:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EDI
	PUSH	ESI
	PUSH	EBX
	MOV	EBX,_timerctl+12
	MOV	EDI,DWORD [8+EBP]
	CALL	_io_load_eflags
	MOV	ESI,EAX
	CALL	_io_cli
L45:
	CMP	BYTE [8+EBX],0
	JE	L42
	CMP	BYTE [9+EBX],0
	JE	L42
	CMP	DWORD [12+EBX],EDI
	JE	L48
L42:
	ADD	EBX,20
	CMP	EBX,_timerctl+9992
	JLE	L45
	MOV	DWORD [8+EBP],ESI
	LEA	ESP,DWORD [-12+EBP]
	POP	EBX
	POP	ESI
	POP	EDI
	POP	EBP
	JMP	_io_store_eflags
L48:
	PUSH	EBX
	CALL	_timer_cancel
	PUSH	EBX
	CALL	_timer_free
	POP	EAX
	POP	EDX
	JMP	L42
	GLOBAL	_inthandler20
_inthandler20:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	ESI
	PUSH	EBX
	XOR	ESI,ESI
	PUSH	96
	PUSH	32
	CALL	_io_out8
	MOV	EAX,DWORD [_timerctl]
	INC	EAX
	MOV	DWORD [_timerctl],EAX
	CMP	DWORD [_timerctl+4],EAX
	POP	EDX
	POP	ECX
	JA	L49
	MOV	EBX,DWORD [_timerctl+8]
L51:
	MOV	EAX,DWORD [4+EBX]
	CMP	EAX,DWORD [_timerctl]
	JA	L52
	MOV	BYTE [8+EBX],1
	CMP	EBX,DWORD [_task_timer]
	JE	L55
	PUSH	DWORD [16+EBX]
	PUSH	DWORD [12+EBX]
	CALL	_fifo32_put
	POP	ECX
	POP	EAX
L56:
	MOV	EBX,DWORD [EBX]
	JMP	L51
L55:
	MOV	ESI,1
	JMP	L56
L52:
	MOV	DWORD [_timerctl+4],EAX
	MOV	DWORD [_timerctl+8],EBX
	MOV	EAX,ESI
	TEST	AL,AL
	JNE	L58
L49:
	LEA	ESP,DWORD [-8+EBP]
	POP	EBX
	POP	ESI
	POP	EBP
	RET
L58:
	LEA	ESP,DWORD [-8+EBP]
	POP	EBX
	POP	ESI
	POP	EBP
	JMP	_task_switch
	GLOBAL	_timerctl
[SECTION .data]
	ALIGNB	16
_timerctl:
	RESB	10012
