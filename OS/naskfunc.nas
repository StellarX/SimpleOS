; naskfunc
; TAB=4

[FORMAT "WCOFF"]				; �����?�����I�͎�	
[BITS 32]						; ����32�ʖ͎��p�I����?��


; �����?�����I�M��

[FILE "naskfunc.nas"]			; ���������M��

		GLOBAL	_io_hlt			; ��������ܓI������


; �ȉ���??�I����

[SECTION .text]		; ��?�������ʗ�?���V�@�Ďʒ���

_io_hlt:	; void io_hlt(void);
		HLT
		RET
