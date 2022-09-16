	ORG $100	; ORG directive must be the first line of code in an assembly file
	
; count.s - Counts down from 10 to 0
	
	LDA #10		; Initalize counter.
	OUT			; Loop: Print counter value.
	SUB #1		; Decrement it.
	JNE $102	; If counter is not zero, then loop.
	out
	HALT
