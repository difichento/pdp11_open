000000:		.= 200
000200:		A: .WORD 34, 12, -1, 66
	 000034
	 000012
	 177777
	 000066
000210:		N: .WORD 4
	 000004
000212:		.= 1000
001000:		mov @#N, R1
	 013701
	 000210
001004:		mov #A, R2
	 012702
	 000200
001010:		clr R0
	 005000
001012:		LOOP:
001012:		mov (R2)+, R3
	 012203
001014:		add R3, R0
	 060300
001016:		sob R1, LOOP
	 077103
001020:		halt
	 000000