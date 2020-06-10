;
; Complete instruction set
; test file for gavrasm compiler
;
; If testhigh is 0 the lower range of values is tested,
; if testhigh is 1 the upper range of values is tested
;
.EQU testhigh=0
;
.IF testhigh==0 ; Lower range of values
.DEF r = R0
.DEF rh = R16
.DEF rd = R24
.DEF rf = R16
.DEF rm = R0
.EQU p = 0
.EQU pl = 0
.EQU b = 0
.EQU k63 = 0
.EQU k127 = 0
.EQU k255 = 0
.EQU k4095 = 0
.EQU k65535 = 0
.EQU k4M = 0
.ELSE ; Upper range of values
.DEF r = R31
.DEF rh = R31
.DEF rd = R30
.DEF rf = R23
.DEF rm = R30
.EQU p = 63
.EQU pl = 31
.EQU b = 7
.EQU k63 = 63
.EQU k127 = -63
.EQU k255 = 255
.EQU k4095 = -2048
.EQU k65535 = 65535
.EQU k4M = 4194303
.ENDIF
;
instruct:
	adc r,r
	add r,r
	adiw rd,k63
	and r,r
	andi rh,k255
	asr r
	bclr b
	bld r,b
.IF testhigh==0
	brbc b,-64
	brbs b,-64
	brcc -64
	brcs -64
.ELSE
	brbc b,+63
	brbs b,+63
	brcc +63
	brcs +63
.ENDIF
        break
.IF testhigh==0
	breq -64
	brge -64
	brhc -64
	brhs -64
	brid -64
	brie -64
	brlo -64
	brlt -64
	brmi -64
	brne -64
	brpl -64
	brsh -64
	brtc -64
	brts -64
	brvc -64
	brvs -64
.ELSE
	breq +63
	brge +63
	brhc +63
	brhs +63
	brid +63
	brie +63
	brlo +63
	brlt +63
	brmi +63
	brne +63
	brpl +63
	brsh +63
	brtc +63
	brts +63
	brvc +63
	brvs +63
.ENDIF
	bset b
	bst r,b
	call k4M
	cbi pl,b
	cbr rh,k255
	clc
	clh
	cli
	cln
	clr r
	cls
	clt
	clv
	clz
	com r
	cp r,r
	cpc r,r
	cpi rh,k255
	cpse r,r
	dec r
        eicall
        eijmp
	elpm
        elpm r,Z
        elpm r,Z+
	eor r,r
        fmul rf,rf
        fmuls rf,rf
        fmulsu rf,rf
	icall
	ijmp
	in r,p
	inc r
	jmp k4M
        ld r,X
	ld r,X+
	ld r,-X
	ld r,Y
	ld r,Y+
	ld r,-Y
	ld r,Z
	ld r,Z+
	ld r,-Z
	ldd r,Y+k63
	ldd r,Z+k63
	ldi rh,k255
	lds r,k65535
	lpm
        lpm r,Z
        lpm r,Z+
	lsl r
	lsr r
	mov r,r
        movw rm,rm
	mul r,r
        muls rh,rh
	neg r
	nop
	or r,r
	ori rh,k255
	out p,r
	pop r
	push r
.IF testhigh==0
	rcall -2048
.ELSE
	rcall +2047
.ENDIF
	ret
	reti
.IF testhigh==0
	rjmp -2048
.ELSE
	rcall +2047
.ENDIF
	rol r
	ror r
	sbc r,r
	sbci rh,k255
	sbi pl,b
	sbic pl,b
	sbis pl,b
	sbiw rd,k63
	sbr rh,k255
	sbrc r,b
	sbrs r,b
	sec
	seh
	sei
	sen
	ser rh
	ses
	set
	sev
	sez
	sleep
	spm
	spm Z+
	st X,r
	st X+,r
	st -X,r
	st Y,r
	st Y+,r
	st -Y,r
	st Z,r
	st Z+,r
	st -Z,r
	std Y+k63,r
	std Z+k63,r
	sts k65535,r
	sub r,r
	subi rh,k255
	swap r
	tst r
	wdr
.EXIT

This is the complete instruction set in one file.
