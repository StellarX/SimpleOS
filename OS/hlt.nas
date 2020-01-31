[BITS 32]
		MOV		AL,'A'
		CALL    2*8:0xBFD
fin:
		RETF
		JMP		fin
		
		