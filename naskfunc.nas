; naskfunc
; TAB=4

[FORMAT "WCOFF"]				; �I�u�W�F�N�g�t�@�C������郂�[�h	
[BITS 32]						; 32�r�b�g���[�h�p�̋@�B�����点��
[INSTRSET "i486p"]	

; �I�u�W�F�N�g�t�@�C���̂��߂̏��

[FILE "naskfunc.nas"]			; �\�[�X�t�@�C�������

		GLOBAL	_io_hlt, _io_cli, _io_sti, _io_stihlt		; ���̃v���O�����Ɋ܂܂��֐���
		GLOBAL	_io_in8, _io_in16, _io_in32
		GLOBAL	_io_out8, _io_out16, _io_out32
		GLOBAL	_io_load_eflags, _io_store_eflags
		GLOBAL	_write_mem8
		GLOBAL	_load_gdtr, _load_idtr
		GLOBAL	_load_cr0, _store_cr0
        GLOBAL  _load_tr
		GLOBAL	_asm_inthandler20,_asm_inthandler21, _asm_inthandler27, _asm_inthandler2c
		GLOBAL	_memtest_sub
        GLOBAL  _farjmp
		EXTERN _inthandler20,_inthandler21, _inthandler27, _inthandler2c

; �ȉ��͎��ۂ̊֐�

[SECTION .text]		; �I�u�W�F�N�g�t�@�C���ł͂���������Ă���v���O����������

_io_hlt:	; void io_hlt(void);
		HLT
		RET
		
_io_cli:
		CLI
		RET
		
_io_sti:
		STI
		RET
		
_io_stihlt:
		STI
		HLT
		RET

_io_in8:
		MOV		EDX,[ESP+4]
		MOV		EAX,0
		IN		AL, DX
		RET	
		
_io_in16:
		MOV		EDX,[ESP+4]
		MOV		EAX,0
		IN		AX, DX
		RET
		
_io_in32:
		MOV		EDX,[ESP+4]
		MOV		EAX,0
		IN		EAX, DX
		RET	

_io_out8:
		MOV		EDX,[ESP+4]
		MOV		AL,[ESP+8]
		OUT		DX, AL
		RET	
		
_io_out16:
		MOV		EDX,[ESP+4]
		MOV		AX,[ESP+8]
		OUT		DX, AX
		RET	

_io_out32:
		MOV		EDX,[ESP+4]
		MOV		EAX,[ESP+8]
		OUT		DX, EAX
		RET		

;�ceflggs�I?��?��?�C�R�@�Ę�?���c���I??�o��?EAX�ԉ�
_io_load_eflags:
		PUSHFD		; push EFLAGS
		POP		EAX
		RET		
		
;�cEAX�I??��?�C�Ĕc?��???eflags		
_io_store_eflags:
		MOV		EAX,[ESP + 4 ]
		PUSH	EAX
		POPFD
		RET
		
_write_mem8:
		MOV		ECX,[ESP+4]		;��꘢�`�Q�F�����n��
		MOV		AL,[ESP+8]		;��񘢌`�Q�F ?
		MOV		[ECX],AL
		RET
		
_load_gdtr:		; void load_gdtr(int limit, int addr);
		MOV		AX,[ESP+4]		; limit
		MOV		[ESP+6],AX
		LGDT	[ESP+6]
		RET

_load_idtr:		; void load_idtr(int limit, int addr);
		MOV		AX,[ESP+4]		; limit
		MOV		[ESP+6],AX
		LIDT	[ESP+6]
		RET
		
_load_cr0:
		MOV 	EAX,CR0
		RET
		
_store_cr0:
		MOV 	EAX, [ESP + 4]
		MOV 	CR0, EAX
		RET
_load_tr:       ;void load_tr(int tr);
        LTR     [ESP + 4]
        RET
        
_asm_inthandler20:
		PUSH		ES
		PUSH		DS
		PUSHAD
		MOV			EAX,ESP
		PUSH		EAX
		MOV 		AX,SS
		MOV			DS,AX
		MOV			ES,AX
		CALL		_inthandler20
		POP			EAX
		POPAD
		POP			DS
		POP			ES
		IRETD        
        
_asm_inthandler21:
		PUSH		ES
		PUSH		DS
		PUSHAD
		MOV			EAX,ESP
		PUSH		EAX
		MOV 		AX,SS
		MOV			DS,AX
		MOV			ES,AX
		CALL		_inthandler21
		POP			EAX
		POPAD
		POP			DS
		POP			ES
		IRETD
		
_asm_inthandler27:
		PUSH		ES
		PUSH		DS
		PUSHAD
		MOV			EAX,ESP
		PUSH		EAX
		MOV 		AX,SS
		MOV			DS,AX
		MOV			ES,AX
		CALL		_inthandler27
		POP			EAX
		POPAD
		POP			DS
		POP			ES
		IRETD
		
_asm_inthandler2c:
		PUSH		ES
		PUSH		DS
		PUSHAD
		MOV			EAX,ESP
		PUSH		EAX
		MOV 		AX,SS
		MOV			DS,AX
		MOV			ES,AX
		CALL		_inthandler2c
		POP			EAX
		POPAD
		POP			DS
		POP			ES
		IRETD
		
_memtest_sub:		; unsigned int memtest_sub( unsigned int start, unsigned int end)
	PUSH		EDI		;�ۑ�?���񑶊�C?�@�ʎg�p?���񑶊�@�\?��?
	PUSH		ESI
	PUSH		EBX
	MOV			ESI, 0xaa55aa55		;pat0
	MOV			EDI, 0x55AA55AA		;pat1
	MOV			EAX, [ESP + 12 + 4]	; i = start		;?���I12??���FEDI ESI EBA�C4���ԉ�n��
mts_loop:
	MOV 		EBX, EAX
	ADD 		EBX, 0xFFC						; p = i + 0xffc
	MOV			EDX, [EBX]						;old = *p
	MOV			[EBX], ESI
	XOR			DWORD [EBX], 0xffffffff			;*p ^= 0xffffffff
	CMP			EDI, [EBX]						;if( *p != pat1) goto finl
	JNE			mts_fin
	XOR			DWORD [EBX], 0xffffffff			;*p ^= 0xffffffff
	CMP			ESI, [EBX]						;if( *p != pat0) goto finl
	JNE			mts_fin
	MOV			[EBX], EDX						;*p = old
	ADD			EAX, 0x1000						; i += 0x1000
	CMP			EAX,[ESP + 12 + 8]				;if( i <= end ) goto mts_loop
	JBE			mts_loop
	POP			EBX								;��???
	POP			ESI
	POP			EDI
	RET
mts_fin:
	MOV			[EBX], EDX						;*p = old
	POP			EBX								;��???
	POP			ESI
	POP			EDI
	RET
    
_farjmp:	; void farjmp(int eip, int cs);
		JMP		FAR [ESP + 4]
		RET
        

		

		
