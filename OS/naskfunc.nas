; naskfunc
; TAB=4

[FORMAT "WCOFF"]				; §ìÚ?¶IÍ®	
[INSTRSET "i486p"]				; gp486½ß
[BITS 32]						; §ì32ÊÍ®pI÷í?¾



; §ìÚ?¶IM§

[FILE "naskfunc.nas"]			; ¹¶¼M§

		GLOBAL	_io_hlt,_write_mem8	; öïÜI¼


; Èº¥I??

[SECTION .text]		; Ú?¶Ê¹?±V@ÄÊö

_io_hlt:	; void io_hlt(void);
		HLT
		RET
		
_write_mem8:	; void write_mem8(int addr, int data);
		MOV		ECX,[ESP+4]		; [ESP + 4]¶úI¥n¬C«´?üECX
		MOV		AL,[ESP+8]		; [ESP + 8]¶úI¥C«´?üAL
		MOV		[ECX],AL
		RET