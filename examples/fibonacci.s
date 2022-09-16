	ORG 200	; Zero page is used by the system for stack space and other purposes
	
; fibonacci.s - A program to compute the first 10 Fibonacci numbers
;
; Address VarName
;------------------
; 22		count 100
; 23		n1		102
; 24		n2		104
; 25		n3		106
	
	LDA #10		; Initialize counter. counter = 10.
	STA 100		;
	LDA #1		; Initialize n2. n2 = 1.
	STA 104		;
	LDA 102		; Loop: n3 = n1 + n2.
	ADD 104		;
	STA 106		;

	LDA 106		; n3 holds the result. Print n3.
	OUT 		; 200
	LDA 104		; n1= n2.
	STA 102		;
	LDA 106		; n2 = n3.
	STA 104		; 210

	LDA 100		; Load counter value.
	SUB #1		; Decrement it & store it.
	STA 100		;
	JNE 210		; If counter is zero, then halt, we're done.
	HALT