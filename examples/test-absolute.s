	ORG $100	; ORG directive must be the first line of code in an assembly file
; test-absolute.s - Tests the absolute LDA instruction
	LDA #20		; Load accumulator with a value.
	STA 25		; Store in memory address 25.
	LDA 25		; Load memory address 25.
	OUT			; Print accumulator.
	HALT		; Stop.
