	.extern sRR
	.extern GOV
	.entry END
	.extern sRR
MAIN:		mov	S1.1, LENGTH
		add	r2,STR
LOOP+:		jmp	END
		macro m1
			inc	K
			mov	S1.2,r3
		endmacro
		prn	#-5
		sub	r1,r4
		m1
		bne	LOOP
		rts r1
		mov
		add r1
		bne a.a
		get #5
		lea #4,r1
		mov r1,#1
		.string ""
2END:		hlt
OMER:		add E23.1s,str
r4:			LOOP
sTR:		.string "abcdef"
LENGTH123456789101112131415102214142:		.data	6,-9,15
ARRR:		.data	22, , 4
S1::		.struct	8,"ab"
abc:		.struct a."ab"
def:		.struct 8,ab
ghi:		.struct 1, ,"hahahah"
ABC:		.string "sda
DEf:		.string ddd"
