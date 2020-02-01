[INSTRSET "i486p"]
[BITS 32]
		MOV		EDX,2	;显示字符串
		MOV		EBX,msg
		INT		0x40
		RETF
msg:
		DB	"hello",0
