[INSTRSET "i486p"]
[BITS 32]
	MOV		EBX,2
	MOV		ECX,msg
	INT		0x40	
	RETF
msg:
	DB		"hello",0