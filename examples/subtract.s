		ORG $100	; ORG directive must be the first line of code in an assembly file
; Deomstrates the use of two's complement. The answer printed to the console should
; be 254, which is -2.
	
	lda #1
	sub #3
	out
	HALT
