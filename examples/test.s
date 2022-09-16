	ORG $100	; ORG directive must be the first line of code in an assembly file

	JSR $104
	HALT
	
	LDA #65
	OUT
	STA $17F
	LDA $17F
	OUT
	ADD #1
	OUT1
	ADD $17F
	OUT
	SUB #1
	OUT
	SUB $17F
	OUT
	MUL #2
	OUT
	DIV #2
	OUT
	LDA #$FF
	PUSHA
	POPA
	OUT
	LDA #1
	SHL
	OUT
	SHR
	OUT
	INC
	OUT
	DEC
	OUT
	NOP
	NOP
	NOP
	LDX #$0C
	INX
	DEX
	STX $00
	LDY #$0D
	INY
	DEY
	STY $01
	DUP
	DUP
	SWAP
	POPA
	POPA
	STA $0
	SEC
	LDA #10
	SUB #1
	OUT
	RTS		