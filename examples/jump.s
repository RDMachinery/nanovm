	ORG $100	; ORG directive must be the first line of code in an assembly file
	
	; Demonstrates the use of the indirect JMP instruction
	
	LDA #01			; High byte of address
	STA $130		; Store it
	LDA #$11		; Low byte of address
	STA $131		; Store it
	JMP ($130)		; Indirect JMP to address $0111 stored at memory address $130 and $131
	OUT				; Print accumulator - never called
	JMP $100		; Jump back to start of code - never called
	HALT			; Address $0111
	