; naskfunc
; TAB=4

[FORMAT "WCOFF"]				; �����?�����I�͎�	
[INSTRSET "i486p"]				; �g�p486����
[BITS 32]						; ����32�ʖ͎��p�I����?��



; �����?�����I�M��

[FILE "naskfunc.nas"]			; ���������M��

		GLOBAL	_io_hlt,_write_mem8	; ��������ܓI������


; �ȉ����I??����

[SECTION .text]		; ��?�������ʗ�?���V�@�Ďʒ���

_io_hlt:	; void io_hlt(void);
		HLT
		RET
		
_write_mem8:	; void write_mem8(int addr, int data);
		MOV		ECX,[ESP+4]		; [ESP + 4]�������I���n���C����?��ECX
		MOV		AL,[ESP+8]		; [ESP + 8]�������I�������C����?��AL
		MOV		[ECX],AL
		RET