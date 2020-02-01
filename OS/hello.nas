[INSTRSET "i486p"]
[BITS 32]
        MOV     ECX,msg
		MOV     EDX,1	;表示显示单个字符
putloop:
        MOV     AL,[CS:ECX]
        CMP     AL,0
        JE      fin
        INT     0x40
        ADD     ECX,1
        JMP     putloop
fin:
        RETF
msg:
        DB  "hello",0