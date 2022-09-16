	ORG $100	; ORG directive must be the first line of code in an assembly file
	; Demonstrates using the CMP instruction
	
	LDX #5		; Load counter limit
	STX $150	; Store in memory
	
	LDA #10		; Load loop start count
	OUT			; Print
	SUB #1		; Loop: Decrement it
	TAY
	CPY $150	; Compare to counter limit
	JNE $107	; If not equal, goto loop.
	OUT
	HALT
	
	