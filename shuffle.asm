.ORIG x3000
	LEA R0, MASK
	LDW R0, R0, #0
	LDB R0, R0, #0   ;load mask to R0

	LEA R4, OLD
	LDW R4, R4, #0   ;load address of old value to R4

	LEA R5, NEW
	LDW R5, R5, #0   ;load address of new value to R5

	AND R1, R1, #0
	ADD R1, R1, #3   ;R1 = 0b11

	AND R1, R1, R0
	ADD R1, R1, #1   ;R1 varies from 1 to 4

LOOP ADD R1, R1, #-1
	BRZ HIT
	ADD R4, R4, #1
	BR LOOP
HIT LDB R6, R4, #0
	STB R6, R5, #0   ;Store the correct value to x4005

	ADD R5, R5, #1   ;Set R5 to x4006

	LEA R4, OLD
	LDW R4, R4, #0   ;RESET address of R4

	AND R1, R1, #0
	ADD R1, R1, #12  ;set R1 to 0b1100
	AND R1, R1, R0
	RSHFL R1, R1, #2
	ADD R1, R1, #1   ;R1 varies from 1 to 4

LOOP ADD R1, R1, #-1
	BRZ HIT
	ADD R4, R4, #1
	BR LOOP
HIT LDB R6, R4, #0
	STB R6, R5, #0   ;Store the correct value to x4006

	ADD R5, R5, #1   ;Set R5 to x4007

	LEA R4, OLD
	LDW R4, R4, #0   ;RESET address of R4

	AND R1, R1, #0
	ADD R1, R1, #3
	LSHF R1, R1, #4  ;set R1 to 0b110000
	AND R1, R1, R0
	RSHFL R1, R1, #4
	ADD R1, R1, #1   ;R1 varies from 1 to 4

LOOP ADD R1, R1, #-1
	BRZ HIT
	ADD R4, R4, #1
	BR LOOP
HIT LDB R6, R4, #0
	STB R6, R5, #0   ;Store the correct value to x4007

	ADD R5, R5, #1   ;Set R5 to x4008

	LEA R4, OLD
	LDW R4, R4, #0   ;RESET address of R4

	AND R1, R1, #0
	ADD R1, R1, #3
	LSHF R1, R1, #6  ;set R1 to 0b11000000
	AND R1, R1, R0
	RSHFL R1, R1, #6
	ADD R1, R1, #1   ;R1 varies from 1 to 4

LOOP ADD R1, R1, #-1
	BRZ HIT
	ADD R4, R4, #1
	BR LOOP
HIT LDB R6, R4, #0
	STB R6, R5, #0   ;Store the correct value to x4008

OLD .FILL x4000
MASK .FILL x4004
NEW .FILL x4005
HALT
.END