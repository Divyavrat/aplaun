;===============================================================================
; Opcode Generator (8086)
;===============================================================================

	; public static constants
	; ----------------------------------------------------------------------
	OG:

		.IB		EQU 010h ; immediate byte (ib <byte>)
		.IW		EQU 020h ; immediate word (iw <word>)
		.GBIW		EQU 021h ; immediate word (but must get data from byte imm stack)
		.WB		EQU 030h ; immediate byte (wb <byte>)
		.WW		EQU 040h ; immediate word (ww <word>)

		.BN		EQU 050h ; /n (r/m)
		.B0		EQU 050h ; /0 (r/m)
		.B1		EQU 051h ; /1 (r/m)
		.B2		EQU 052h ; /2 (r/m)
		.B3		EQU 053h ; /3 (r/m)
		.B4		EQU 054h ; /4 (r/m)
		.B5		EQU 055h ; /5 (r/m)
		.B6		EQU 056h ; /6 (r/m)
		.B7		EQU 057h ; /7 (r/m)

		.BR		EQU 060h ; /r
		.RM1		EQU 061h ; /r1 (r/m for first argument)
		.RM2		EQU 062h ; /r2 (r/m for second argument)

		.RW		EQU 070h ; relative adress WORD
		.RB		EQU 071h ; relative adress BYTE
		.GBRW		EQU 072h ; relative adress WORD (but must get data from byte imm stack)

		.OW		EQU 080h ; ? offset (value of offset ?)

		.PR		EQU 090h ; XX+r 

		.A2MBIMM1	EQU 091h ; arg 2 must be imm 1
		.A2MBCL		EQU 092h ; arg 2 must be CL

	; public static properties
	; ----------------------------------------------------------------------
	OpcodeGenerator:
		.MAX_SIZE			EQU 5567 	; can generate max 5k opcodes
		.offset				dw 32768 	; default value for org instruction
		.startMemoryAddress		dw 54768
		.index				dw 0		; index of generated opcode

	; private static properties
	; ----------------------------------------------------------------------
	private.OpcodeGenerator:
		.generationInstructionAddress	dw 0		; address of generation instructions
		.generationInstructionIndex	dw 0		; index of generation instructions

		; stack for immediate byte and word (ib & iw) FIFO
		.ibPushIndex			db 0		; can be 0 or 1
		.ibPopIndex			db 0		; can be 0 or 1
		.ib1				dw 0		; ib1
		.ib2				dw 0		; ib2

		.iwPushIndex			db 0		; can be 0 or 1
		.iwPopIndex			db 0		; can be 0 or 1

		.iw1				dw 0		; iw1
		.iw1IsLabel			db 0		; 0->imm16 1->label
		.iw1LabelName			times 14 db 0

		.iw2				dw 0		; iw2
		.iw2IsLabel			db 0		; 0->imm16 1->label
		.iw2LabelName			times 14 db 0

		; stack for R/M FIFO
		.rmPushIndex			db 0		; can be 0 or 1
		.rmPopIndex			db 0		; can be 0 or 1

		.rmId1				db 0
		.rmType1			db 0
		.rmRegIndex1			db 0
		.rmRM1				db 0
		.rmHasExtraImm8_1		db 0		; 0=false 1=true
		.rmExtraImm8_1			db 0		
		.rmHasExtraImm16_1		db 0		; 0=false 1=true
		.rmExtraImm16_1			dw 0
		.rmExtraImm16IsLabel_1		dw 0		; 0=false 1=true (di)

		.rmId2				db 0
		.rmType2			db 0
		.rmRegIndex2			db 0
		.rmRM2				db 0
		.rmHasExtraImm8_2		db 0		; 0=false 1=true
		.rmExtraImm8_2			db 0		
		.rmHasExtraImm16_2		db 0		; 0=false 1=true
		.rmExtraImm16_2			dw 0
		.rmExtraImm16IsLabel_2		dw 0		; 0=false 1=true (di)
	

	; public static methods
	; ----------------------------------------------------------------------
	
		OpcodeGenerator.execute:
		; --------------------------------------------------------------
		; ax = memory address of opcode generation instructions
		; bl = 0 - invalid instruction !
		;      1 - success
		;      2 - invalid argument !
				mov	word [private.OpcodeGenerator.generationInstructionIndex], 1
				mov	word [private.OpcodeGenerator.generationInstructionAddress], ax
				call	private.OpcodeGenerator.debug.printIndex
			.nextInstruction:
				call	private.OpcodeGenerator.getNextInstruction
				cmp	bl, 0 ; end of line
				je	.success

				cmp	al, OG.WB
				je	private.OpcodeGenerator.handle_INSTR_WB

				cmp	al, OG.WW
				je	private.OpcodeGenerator.handle_INSTR_WW

				cmp	al, OG.IB
				je	private.OpcodeGenerator.handle_INSTR_IB

				cmp	al, OG.IW
				je	private.OpcodeGenerator.handle_INSTR_IW

				cmp	al, OG.GBIW
				je	private.OpcodeGenerator.handle_INSTR_GBIW

				cmp	al, OG.RM1
				je	private.OpcodeGenerator.handle_INSTR_RM1

				cmp	al, OG.RM2
				je	private.OpcodeGenerator.handle_INSTR_RM2

				cmp	al, OG.PR
				je	private.OpcodeGenerator.handle_INSTR_PLUS_R

				cmp	al, OG.A2MBIMM1
				je	private.OpcodeGenerator.handle_INSTR_A2MBIMM1

				cmp	al, OG.A2MBCL
				je	private.OpcodeGenerator.handle_INSTR_A2MBCL

				cmp	al, OG.RB
				je	private.OpcodeGenerator.handle_INSTR_RB

				cmp	al, OG.RW
				mov	ah, 1
				je	private.OpcodeGenerator.handle_INSTR_RW

				cmp	al, OG.GBRW
				mov	ah, 2
				je	private.OpcodeGenerator.handle_INSTR_GBRW


				mov	ah, al
				and	ah, 0F0h
				cmp	ah, OG.BN
				je	private.OpcodeGenerator.handle_INSTR_BN

			.success:
				mov	bl, 1
				ret
			.invalidInstruction:
				mov	bl, 0
				ret
			.invalidArgument:
				mov	bl, 2
				ret

		OpcodeGenerator.clearIbIwRMStacks:
		; --------------------------------------------------------------
		; bl = 0 - error
		;      1 - success
				mov	byte [private.OpcodeGenerator.ibPushIndex], 0
				mov	byte [private.OpcodeGenerator.ibPopIndex], 0
				mov	byte [private.OpcodeGenerator.ib1], 0
				mov	byte [private.OpcodeGenerator.ib2], 0
				mov	byte [private.OpcodeGenerator.iwPushIndex], 0
				mov	byte [private.OpcodeGenerator.iwPopIndex], 0
				mov	word [private.OpcodeGenerator.iw1], 0
				mov	byte [private.OpcodeGenerator.iw1IsLabel], 0
				mov	word [private.OpcodeGenerator.iw2], 0
				mov	byte [private.OpcodeGenerator.iw2IsLabel], 0

				mov	cx, 14
				mov	bx, private.OpcodeGenerator.iw1LabelName
			.clearIw1LabelName:
				mov	byte [bx], 0
				loop	.clearIw1LabelName

				mov	cx, 14
				mov	bx, private.OpcodeGenerator.iw2LabelName
			.clearIw2LabelName:
				mov	byte [bx], 0
				loop	.clearIw2LabelName

				mov	byte [private.OpcodeGenerator.rmPushIndex], 0
				mov	byte [private.OpcodeGenerator.rmPopIndex], 0
				mov	byte [private.OpcodeGenerator.rmId1], 0
				mov	byte [private.OpcodeGenerator.rmType1], 0
				mov	byte [private.OpcodeGenerator.rmRegIndex1], 0
				mov	byte [private.OpcodeGenerator.rmRM1], 0
				mov	byte [private.OpcodeGenerator.rmHasExtraImm8_1], 0
				mov	byte [private.OpcodeGenerator.rmExtraImm8_1], 0
				mov	byte [private.OpcodeGenerator.rmHasExtraImm16_1], 0
				mov	word [private.OpcodeGenerator.rmExtraImm16_1], 0
				mov	word [private.OpcodeGenerator.rmExtraImm16IsLabel_1], 0
				mov	byte [private.OpcodeGenerator.rmId2], 0
				mov	byte [private.OpcodeGenerator.rmType2], 0
				mov	byte [private.OpcodeGenerator.rmRegIndex2], 0
				mov	byte [private.OpcodeGenerator.rmRM2], 0
				mov	byte [private.OpcodeGenerator.rmHasExtraImm8_2], 0
				mov	byte [private.OpcodeGenerator.rmExtraImm8_2], 0
				mov	byte [private.OpcodeGenerator.rmHasExtraImm16_2], 0
				mov	word [private.OpcodeGenerator.rmExtraImm16_2], 0
				mov	word [private.OpcodeGenerator.rmExtraImm16IsLabel_2], 0
				mov	bl, 1
				ret

		OpcodeGenerator.pushIb:
		; --------------------------------------------------------------
		; al = ib
		; bl = 0 - stack overflow
		;      1 - success
				mov	bl, [private.OpcodeGenerator.ibPushIndex]
				cmp	bl, 0
				je	.addIb1
				cmp	bl, 1
				je	.addIb2
				jmp	short .stackOverflow
			.addIb1:						
				mov	byte [private.OpcodeGenerator.ib1], al
				jmp	.success
			.addIb2:						
				mov	byte [private.OpcodeGenerator.ib2], al
			.success:
				inc	byte [private.OpcodeGenerator.ibPushIndex]
				mov bl, 1
				ret
			.stackOverflow:
				mov bl, 0
				ret

		OpcodeGenerator.pushIw:
		; --------------------------------------------------------------
		; ax = iw
		; cl = 0->imm16 1->is label
		; dx = label name string pointer
		; ret bl = 0 - stack overflow
		;          1 - success
				mov	si, dx
				mov	bl, [private.OpcodeGenerator.iwPushIndex]
				cmp	bl, 0
				je	.addIw1
				cmp	bl, 1
				je	.addIw2
				jmp	short .stackOverflow
			.addIw1:						
				mov	word [private.OpcodeGenerator.iw1], ax
				mov	byte [private.OpcodeGenerator.iw1IsLabel], cl
				mov	di, private.OpcodeGenerator.iw1LabelName
				call	.copyString
				jmp	.success
			.addIw2:						
				mov	word [private.OpcodeGenerator.iw2], ax
				mov	byte [private.OpcodeGenerator.iw2IsLabel], cl
				mov	di, private.OpcodeGenerator.iw2LabelName
				call	.copyString
			.success:
				inc	word [private.OpcodeGenerator.iwPushIndex]
				mov	bl, 1
				ret
			.stackOverflow:
				mov	bl, 0
				ret
			.copyString:
				mov	al, [si]
				mov	byte [di], al
				cmp	al, 0
				je	.copyString.end
				inc	si
				inc	di
				jmp	short .copyString
			.copyString.end:
				ret
				
		OpcodeGenerator.popIb:
		; --------------------------------------------------------------
		; al = ib
		; bl = 0 - stack overflow
		;      1 - success
				mov	bl, [private.OpcodeGenerator.ibPopIndex]
				cmp	bl, 0
				je	.retIb1
				cmp	bl, 1
				je	.retIb2
				jmp	short .stackOverflow
			.retIb1:						
				mov	al, [private.OpcodeGenerator.ib1]
				jmp	.success
			.retIb2:						
				mov	al, [private.OpcodeGenerator.ib2]
			.success:
				inc	byte [private.OpcodeGenerator.ibPopIndex]
				mov	bl, 1
				ret
			.stackOverflow:
				mov	al, 0
				mov	bl, 0
				ret

		OpcodeGenerator.popIw:
		; --------------------------------------------------------------
		; ax = iw
		; cl = 0->imm16 1->is label
		; dx = label name string pointer
		; bl = 0 - stack overflow
		;      1 - success
				mov	bl, [private.OpcodeGenerator.iwPopIndex]
				cmp	bl, 0
				je	.retIw1
				cmp	bl, 1
				je	.retIw2
				jmp	short .stackOverflow
			.retIw1:						
				mov	ax, [private.OpcodeGenerator.iw1]
				mov	cl, [private.OpcodeGenerator.iw1IsLabel]
				mov	dx, private.OpcodeGenerator.iw1LabelName
				jmp	.success
			.retIw2:						
				mov	ax, [private.OpcodeGenerator.iw2]
				mov	cl, [private.OpcodeGenerator.iw2IsLabel]
				mov	dx, private.OpcodeGenerator.iw2LabelName
			.success:
				inc	byte [private.OpcodeGenerator.iwPopIndex]
				mov	bl, 1
				ret
			.stackOverflow:
				mov	ax, 0
				mov	bl, 0
				ret
		.rmHasExtraImm8_1		db 0		; 0=false 1=true
		.rmExtraImm8_1			db 0		
		.rmHasExtraImm16_1		db 0		; 0=false 1=true
		.rmExtraImm16_1			dw 0

		OpcodeGenerator.pushRM:
		; --------------------------------------------------------------
		;   al = Id
		;   ah = Type
		;   cl = RegIndex
		;   ch = R/M
		;   dl = has extra imm8
		;   dh = extra imm8
		;   bh = has extra imm16
		;   si = extra imm16
		;   di = extra imm16 is label ? 1=true 0=false
		; bl = 0 - stack overflow
		;      1 - success
				mov	bl, [private.OpcodeGenerator.rmPushIndex]
				cmp	bl, 0
				je	.addRM1
				cmp	bl, 1
				je	.addRM2
				jmp	short .stackOverflow
			.addRM1:			
				mov	byte [private.OpcodeGenerator.rmId1], al
				mov	byte [private.OpcodeGenerator.rmType1], ah
				mov	byte [private.OpcodeGenerator.rmRegIndex1], cl
				mov	byte [private.OpcodeGenerator.rmRM1], ch
				mov	byte [private.OpcodeGenerator.rmHasExtraImm8_1], dl
				mov	byte [private.OpcodeGenerator.rmExtraImm8_1], dh
				mov	byte [private.OpcodeGenerator.rmHasExtraImm16_1], bh
				mov	word [private.OpcodeGenerator.rmExtraImm16_1], si
				mov	word [private.OpcodeGenerator.rmExtraImm16IsLabel_1], di
				jmp	.success
			.addRM2:
				mov	byte [private.OpcodeGenerator.rmId2], al
				mov	byte [private.OpcodeGenerator.rmType2], ah
				mov	byte [private.OpcodeGenerator.rmRegIndex2], cl
				mov	byte [private.OpcodeGenerator.rmRM2], ch
				mov	byte [private.OpcodeGenerator.rmHasExtraImm8_2], dl
				mov	byte [private.OpcodeGenerator.rmExtraImm8_2], dh
				mov	byte [private.OpcodeGenerator.rmHasExtraImm16_2], bh
				mov	word [private.OpcodeGenerator.rmExtraImm16_2], si
				mov	word [private.OpcodeGenerator.rmExtraImm16IsLabel_2], di
			.success:
				inc	byte [private.OpcodeGenerator.rmPushIndex]
				mov bl, 1
				ret
			.stackOverflow:
				mov bl, 0
				ret

		OpcodeGenerator.popRM:
		; --------------------------------------------------------------
		; returns:
		;   al = Id
		;   ah = Type
		;   cl = RegIndex
		;   ch = R/M
		;   dl = has extra imm8
		;   dh = extra imm8
		;   bh = has extra imm16
		;   si = extra imm16
		;   di = extra imm16 is label ? 1=true 0=false
		; bl = 0 - stack overflow
		;      1 - success
				mov	bl, [private.OpcodeGenerator.rmPopIndex]
				cmp	bl, 0
				je	.getRM1
				cmp	bl, 1
				je	.getRM2
				jmp	short .stackOverflow
			.getRM1:						
				mov	al, [private.OpcodeGenerator.rmId1]
				mov	ah, [private.OpcodeGenerator.rmType1]
				mov	cl, [private.OpcodeGenerator.rmRegIndex1]
				mov	ch, [private.OpcodeGenerator.rmRM1]
				mov	dl, [private.OpcodeGenerator.rmHasExtraImm8_1]
				mov	dh, [private.OpcodeGenerator.rmExtraImm8_1]
				mov	bh, [private.OpcodeGenerator.rmHasExtraImm16_1]
				mov	si, [private.OpcodeGenerator.rmExtraImm16_1]
				mov	di, [private.OpcodeGenerator.rmExtraImm16IsLabel_1]
				jmp	.success
			.getRM2:						
				mov	al, [private.OpcodeGenerator.rmId2]
				mov	ah, [private.OpcodeGenerator.rmType2]
				mov	cl, [private.OpcodeGenerator.rmRegIndex2]
				mov	ch, [private.OpcodeGenerator.rmRM2]
				mov	dl, [private.OpcodeGenerator.rmHasExtraImm8_2]
				mov	dh, [private.OpcodeGenerator.rmExtraImm8_2]
				mov	bh, [private.OpcodeGenerator.rmHasExtraImm16_2]
				mov	si, [private.OpcodeGenerator.rmExtraImm16_2]
				mov	di, [private.OpcodeGenerator.rmExtraImm16IsLabel_2]
			.success:
				inc	byte [private.OpcodeGenerator.rmPopIndex]
				mov bl, 1
				ret
			.stackOverflow:
				mov bl, 0
				ret


	; private static methods
	; ----------------------------------------------------------------------

		private.OpcodeGenerator.getNextInstruction:
		; --------------------------------------------------------------
		; al = next instruction
		; bl = 0 - end of line
		;      1 - success
				push	cx

				mov	si, [private.OpcodeGenerator.generationInstructionIndex]
				mov	bx, [private.OpcodeGenerator.generationInstructionAddress]

				mov	ch, 0
				mov	cl, [bx] ; size of opcode generation array
				cmp	si, cx
				jg	.endOfLine

				mov	al, [bx + si] ; next instruction

				inc	word [private.OpcodeGenerator.generationInstructionIndex]
			.success:
				pop	cx
				mov	bl, 1
				ret
			.endOfLine:
				pop	cx
				mov	bl, 0
				ret

		private.OpcodeGenerator.writeByte:
		; --------------------------------------------------------------
		; al = byte to write
		; bl = 0 - out of memory
		;      1 - success
				mov	si, [OpcodeGenerator.startMemoryAddress]
				mov	bx, [OpcodeGenerator.index]
				cmp	bx, OpcodeGenerator.MAX_SIZE
				jge	.outOfMemory
				mov	byte [bx + si], al
				inc	word [OpcodeGenerator.index]
			.success:
				mov	bl, 1
				ret
			.outOfMemory:
				mov	bl, 0
				ret

		private.OpcodeGenerator.writeWord:
		; --------------------------------------------------------------
		; ax = word to write
		; bl = 0 - out of memory
		;      1 - success
				mov	si, [OpcodeGenerator.startMemoryAddress]
				mov	bx, [OpcodeGenerator.index]
				mov	dx, bx
				inc	dx
				cmp	dx, OpcodeGenerator.MAX_SIZE
				jge	.outOfMemory
				mov	word [bx + si], ax
				inc	word [OpcodeGenerator.index]
				inc	word [OpcodeGenerator.index]
			.success:
				mov	bl, 1
				ret
			.outOfMemory:
				mov	bl, 0
				ret

	; private static methods (generation instructions handlers)
	; ----------------------------------------------------------------------

		private.OpcodeGenerator.handle_INSTR_IB:
		; --------------------------------------------------------------
				mov	si, msg.handle_INSTR_IB
				call	os_print_string

				call	OpcodeGenerator.popIb ; al=ib
				call	private.OpcodeGenerator.debug.printHexByte
				call	os_print_newline

				call	private.OpcodeGenerator.writeByte
				jmp	OpcodeGenerator.execute.nextInstruction
			msg.handle_INSTR_IB	db	"          ---> writing imm byte: ", 0


		private.OpcodeGenerator.handle_INSTR_IW:
		; --------------------------------------------------------------
				mov	si, msg.handle_INSTR_IW
				call	os_print_string
				call	OpcodeGenerator.popIw ; ax=iw

				; is label ?
				cmp	cl, 1
				jne	.notIsLabel
			.isLabel:
				; ax = pointer to label string
				; dx = address (offset) of label (org)
				; bx = address where value must be corrected
				; cl = LabelResolver.TYPE_XX
				mov	ax, dx
				mov	dx, 0
				mov	bx, [OpcodeGenerator.index]
				mov	cl, LabelResolver.TYPE_IW
				call	LabelResolver.add
				mov	ax, 0
			.notIsLabel:
				call	private.OpcodeGenerator.debug.printHexWord
				call	os_print_newline

				call	private.OpcodeGenerator.writeWord
				jmp	OpcodeGenerator.execute.nextInstruction
			msg.handle_INSTR_IW	db	"          ---> writing imm word: ", 0


		private.OpcodeGenerator.handle_INSTR_GBIW:
		; --------------------------------------------------------------
				mov	si, msg.handle_INSTR_GBIW
				call	os_print_string
				call	OpcodeGenerator.popIb ; al=ib
				mov	ah, 0

				call	private.OpcodeGenerator.debug.printHexWord
				call	os_print_newline

				call	private.OpcodeGenerator.writeWord
				jmp	OpcodeGenerator.execute.nextInstruction
			msg.handle_INSTR_GBIW	db	"          ---> writing imm word (from byte imm stack): ", 0


		private.OpcodeGenerator.handle_INSTR_WB:
		; --------------------------------------------------------------
				mov	si, msg.handle_INSTR_WB
				call	os_print_string

				call	private.OpcodeGenerator.getNextInstruction ; al=next byte
				call	private.OpcodeGenerator.debug.printHexByte
				call	os_print_newline

				call	private.OpcodeGenerator.writeByte
				jmp	OpcodeGenerator.execute.nextInstruction
			msg.handle_INSTR_WB		db	"          ---> writing byte: ", 0


		private.OpcodeGenerator.handle_INSTR_WW:
		; --------------------------------------------------------------
				call	private.OpcodeGenerator.getNextInstruction
				mov	byte [msg.handle_INSTR_WW.word], al

				call	private.OpcodeGenerator.getNextInstruction
				mov	byte [msg.handle_INSTR_WW.word+1], al

				mov	si, msg.handle_INSTR_WW
				call	os_print_string
				call	os_print_newline

				mov	ax, [msg.handle_INSTR_WW.word]
				call	private.OpcodeGenerator.writeWord

				jmp	OpcodeGenerator.execute.nextInstruction
			msg.handle_INSTR_WW		db	"          ---> writing word: "
			msg.handle_INSTR_WW.word	dw	0, 0


		private.OpcodeGenerator.handle_INSTR_BN:
		; --------------------------------------------------------------
				mov	si, msg.handle_INSTR_BN
				call	os_print_string

				and	al, 0Fh
				mov	byte [.regIndex], al
				call	private.OpcodeGenerator.debug.printHexByte
				call	os_print_newline

				;   al = Id
				;   ah = Type
				;   cl = RegIndex
				;   ch = R/M
				;   dl = has extra imm8
				;   dh = extra imm8
				;   bh = has extra imm16
				;   si = extra imm16
				;   di = extra imm16 is label ? 1=true 0=false
				call	OpcodeGenerator.popRM

				pusha

				mov	ah, 0 ;debug
				mov	al, [.regIndex] ; 00000111b
				shl	al, 3  ; 00111000b
				or	al, ch ; ch=XX000XXX + 00111000b

				mov	si, msg.handle_INSTR_BN.wr
				call	os_print_string
				call	private.OpcodeGenerator.debug.printHexByte
				call	os_print_newline
				
				call	private.OpcodeGenerator.writeByte

				popa
				cmp	dl, 1 ; has extra imm8 ?
				je	.writeExtraImm8
				cmp	bh, 1 ; has extra imm16 ?
				je	.writeExtraImm16
				jmp	.success
			.writeExtraImm8:
				mov	al, dh ; dh = extra imm8

				mov	si, msg.handle_INSTR_BN.wrextrb
				call	os_print_string
				call	private.OpcodeGenerator.debug.printHexByte
				call	os_print_newline

				call	private.OpcodeGenerator.writeByte
				jmp	short .success
			.writeExtraImm16:
				mov	ax, si ; si = extra imm16
				mov	[prov.AX], ax

				mov	si, msg.handle_INSTR_BN.wrextrw
				call	os_print_string
				call	private.OpcodeGenerator.debug.printHexWord
				call	os_print_newline

				cmp	di, 1 ; extra imm16 is label ?
				jne	.extraImm16NotIsLabel

			.extraImm16IsLabel:
				call	OpcodeGenerator.popIw ; remove label from iw stack
				; ax = pointer to label string
				; dx = address (offset) of label (org)
				; bx = address where value must be corrected
				; cl = LabelResolver.TYPE_XX
				mov	ax, dx
				mov	dx, 0
				mov	bx, [OpcodeGenerator.index]
				mov	cl, LabelResolver.TYPE_IW
				call	LabelResolver.add

			.extraImm16NotIsLabel:
				mov	ax, [prov.AX]
				call	private.OpcodeGenerator.writeWord
				jmp	short .success
			.success:
				jmp	OpcodeGenerator.execute.nextInstruction
			.regIndex db 0
			msg.handle_INSTR_BN		db	"          ---> /n -> n=", 0
			msg.handle_INSTR_BN.wr		db	"              ---> writing byte: ", 0
			msg.handle_INSTR_BN.wrextrb	db	"              ---> writing extra byte: ", 0
			msg.handle_INSTR_BN.wrextrw	db	"              ---> writing extra word: ", 0
			prov.AX				dw 0

		private.OpcodeGenerator.handle_INSTR_RM1:
		; --------------------------------------------------------------
				mov	si, msg.handle_INSTR_RM1
				call	os_print_string
				call	os_print_newline

				call	OpcodeGenerator.popRM
				mov	word [.provAX], ax
				mov	word [.provBX], bx
				mov	word [.provCX], cx
				mov	word [.provDX], dx
				mov	word [.provDI], di
				mov	word [.provSI], si

				push	cx ; save ch=r/m
		
				call	OpcodeGenerator.popRM ; cl=reg index
				mov	al, cl
				shl	al, 3
				pop	cx
				or	al, ch

				mov	si, msg.handle_INSTR_BN.wr
				call	os_print_string
				call	private.OpcodeGenerator.debug.printHexByte
				call	os_print_newline
				
				call	private.OpcodeGenerator.writeByte

			; -------------------------------------------------------------------
				mov	ax, [.provAX]
				mov	bx, [.provBX]
				mov	cx, [.provCX]
				mov	dx, [.provDX]
				mov	di, [.provDI]
				mov	si, [.provSI]

				cmp	dl, 1 ; has extra imm8 ?
				je	.writeExtraImm8
				cmp	bh, 1 ; has extra imm16 ?
				je	.writeExtraImm16
				jmp	.success
			.writeExtraImm8:
				mov	al, dh ; dh = extra imm8

				mov	si, msg.handle_INSTR_BN.wrextrb
				call	os_print_string
				call	private.OpcodeGenerator.debug.printHexByte
				call	os_print_newline

				call	private.OpcodeGenerator.writeByte
				jmp	short .success
			.writeExtraImm16:
					; ax = iw
					; cl = 0->imm16 1->is label
					; dx = label name string pointer
					; bl = 0 - stack overflow
					call	OpcodeGenerator.popIw ; ax=iw

					cmp	cl, 1 ; imm16 is label ?
					jne	.imm16NotIsLabel

				.imm16IsLabel:

					; ax = pointer to label string
					; dx = address (offset) of label (org)
					; bx = address where value must be corrected
					; cl = LabelResolver.TYPE_XX
					mov	ax, dx
					mov	dx, 0
					mov	bx, [OpcodeGenerator.index]
					mov	cl, LabelResolver.TYPE_IW
					call	LabelResolver.add
					mov	ax, 0
					jmp	short .writeWord
				.imm16NotIsLabel:
					mov	si, [.provSI]
					mov	ax, si ; si = extra imm16
				.writeWord:
					mov	si, msg.handle_INSTR_BN.wrextrw
					call	os_print_string
					call	private.OpcodeGenerator.debug.printHexWord
					call	os_print_newline
					call	private.OpcodeGenerator.writeWord

					jmp	short .success
			.success:

			; -------------------------------------------------------------------


					jmp	OpcodeGenerator.execute.nextInstruction
			.provAX		dw 0 
			.provBX		dw 0 
			.provCX		dw 0 
			.provDX		dw 0 
			.provDI		dw 0 
			.provSI		dw 0 
			msg.handle_INSTR_RM1	db	"          ---> /r1", 0

		private.OpcodeGenerator.handle_INSTR_RM2:
		; --------------------------------------------------------------
				mov	si, msg.handle_INSTR_RM2
				call	os_print_string
				call	os_print_newline

				call	OpcodeGenerator.popRM
				push	cx ; save cl=reg index
				call	OpcodeGenerator.popRM ; ch=r/m

				mov	word [.provAX], ax
				mov	word [.provBX], bx
				mov	word [.provCX], cx
				mov	word [.provDX], dx
				mov	word [.provDI], di
				mov	word [.provSI], si

				pop	ax
				shl	al, 3
				or	al, ch
				
				mov	si, msg.handle_INSTR_BN.wr
				call	os_print_string
				call	private.OpcodeGenerator.debug.printHexByte
				call	os_print_newline
				
				call	private.OpcodeGenerator.writeByte

			; -------------------------------------------------------------------
				mov	ax, [.provAX]
				mov	bx, [.provBX]
				mov	cx, [.provCX]
				mov	dx, [.provDX]
				mov	di, [.provDI]
				mov	si, [.provSI]

				cmp	dl, 1 ; has extra imm8 ?
				je	.writeExtraImm8
				cmp	bh, 1 ; has extra imm16 ?
				je	.writeExtraImm16
				jmp	.success
			.writeExtraImm8:
				mov	al, dh ; dh = extra imm8

				mov	si, msg.handle_INSTR_BN.wrextrb
				call	os_print_string
				call	private.OpcodeGenerator.debug.printHexByte
				call	os_print_newline

				call	private.OpcodeGenerator.writeByte
				jmp	short .success
			.writeExtraImm16:
					; ax = iw
					; cl = 0->imm16 1->is label
					; dx = label name string pointer
					; bl = 0 - stack overflow
					call	OpcodeGenerator.popIw ; ax=iw

					cmp	cl, 1 ; imm16 is label ?
					jne	.imm16NotIsLabel

				.imm16IsLabel:

					; ax = pointer to label string
					; dx = address (offset) of label (org)
					; bx = address where value must be corrected
					; cl = LabelResolver.TYPE_XX
					mov	ax, dx
					mov	dx, 0
					mov	bx, [OpcodeGenerator.index]
					mov	cl, LabelResolver.TYPE_IW
					call	LabelResolver.add
					mov	ax, 0
					jmp	short .writeWord
				.imm16NotIsLabel:
					mov	si, [.provSI]
					mov	ax, si ; si = extra imm16
				.writeWord:
					mov	si, msg.handle_INSTR_BN.wrextrw
					call	os_print_string
					call	private.OpcodeGenerator.debug.printHexWord
					call	os_print_newline
					call	private.OpcodeGenerator.writeWord

					jmp	short .success
			.success:

			; -------------------------------------------------------------------

				jmp	OpcodeGenerator.execute.nextInstruction
			.provAX		dw 0 
			.provBX		dw 0 
			.provCX		dw 0 
			.provDX		dw 0 
			.provDI		dw 0 
			.provSI		dw 0 
			msg.handle_INSTR_RM2	db	"          ---> /r2", 0


		private.OpcodeGenerator.handle_INSTR_PLUS_R:
		; --------------------------------------------------------------
				mov	si, msg.handle_INSTR_PLUS_R
				call	os_print_string

				call	OpcodeGenerator.popRM ; cl=regindex

				call	private.OpcodeGenerator.getNextInstruction ; al=next byte
				add	al, cl
				call	private.OpcodeGenerator.debug.printHexByte
				call	os_print_newline

				call	private.OpcodeGenerator.writeByte
				jmp	OpcodeGenerator.execute.nextInstruction
			msg.handle_INSTR_PLUS_R		db	"          ---> writing byte: ", 0


		private.OpcodeGenerator.handle_INSTR_RB:
		; --------------------------------------------------------------
		; // TODO -> if ax > 0ffh -> error !

				mov	si, msg.handle_INSTR_RB
				call	os_print_string
				call	OpcodeGenerator.popIw ; ax=iw

				; is label ?
				cmp	cl, 1
				jne	.notIsLabel
			.isLabel:
				; ax = pointer to label string
				; dx = address (offset) of label (org)
				; bx = address where value must be corrected
				; cl = LabelResolver.TYPE_XX
				mov	ax, dx
				mov	dx, 0
				mov	bx, [OpcodeGenerator.index]
				mov	cl, LabelResolver.TYPE_RB
				call	LabelResolver.add
				mov	ax, 0
			.notIsLabel:
				call	private.OpcodeGenerator.debug.printHexByte
				call	os_print_newline

				call	private.OpcodeGenerator.writeByte
				jmp	OpcodeGenerator.execute.nextInstruction
			msg.handle_INSTR_RB	db	"          ---> writing rel byte: ", 0


		private.OpcodeGenerator.handle_INSTR_GBRW:
		; --------------------------------------------------------------
				mov ah, 2
				jmp short private.OpcodeGenerator.handle_INSTR_RW


		private.OpcodeGenerator.handle_INSTR_RW:
		; --------------------------------------------------------------
		; ah = 1 -> handle_INSTR_RW
		; ah = 2 -> handle_INSTR_GBRW
				cmp	ah, 1 ; -> handle_INSTR_RW
				je	.handle_INSTR_RW
				jmp	.handle_INSTR_GBRW

			.handle_INSTR_RW:
				mov	si, msg.handle_INSTR_RW
				call	os_print_string
				call	OpcodeGenerator.popIw ; ax=iw
				jmp	short .verifyIsLabel

			.handle_INSTR_GBRW:
				mov	si, msg.handle_INSTR_GBRW
				call	os_print_string
				call	OpcodeGenerator.popIb ; al=ib
				mov	ah, 0

			.verifyIsLabel:
				; is label ?
				cmp	cl, 1
				jne	.notIsLabel

			.isLabel:
				; ax = pointer to label string
				; dx = address (offset) of label (org)
				; bx = address where value must be corrected
				; cl = LabelResolver.TYPE_XX
				mov	ax, dx
				mov	dx, 0
				mov	bx, [OpcodeGenerator.index]
				mov	cl, LabelResolver.TYPE_RW
				call	LabelResolver.add
				mov	ax, 0
				jmp	short .write

			.notIsLabel:
				mov	bx, [OpcodeGenerator.offset]
				add	bx, [OpcodeGenerator.index]
				add	bx, 2
				sub	ax, bx

			.write:
				call	private.OpcodeGenerator.debug.printHexWord
				call	os_print_newline
				call	private.OpcodeGenerator.writeWord

				jmp	OpcodeGenerator.execute.nextInstruction
			msg.handle_INSTR_RW	db	"          ---> writing rel word: ", 0
			msg.handle_INSTR_GBRW	db	"          ---> writing rel word (GBRW): ", 0

		private.OpcodeGenerator.handle_INSTR_A2MBIMM1:
		; --------------------------------------------------------------
				mov	si, msg.handle_INSTR_A2MBIMM1
				call	os_print_string
				mov	ax, [private.OpcodeGenerator.ib1]
				cmp	ax, 1
				jne	.error
			.ok:
				mov	si, msg.handle_INSTR_A2MBIMM1.ok
				call	os_print_string
				call	os_print_newline
				jmp	OpcodeGenerator.execute.nextInstruction
			.error:
				jmp	OpcodeGenerator.execute.invalidArgument

			msg.handle_INSTR_A2MBIMM1 	db "          ---> Arg2 must be 1 ... ", 0
			msg.handle_INSTR_A2MBIMM1.ok	db "ok", 0

		private.OpcodeGenerator.handle_INSTR_A2MBCL:
		; --------------------------------------------------------------
				mov	si, msg.handle_INSTR_A2MBCL
				call	os_print_string
				mov	al, [private.OpcodeGenerator.rmId2]
				cmp	al, ID_CL
				jne	.error
			.ok:
				mov	si, msg.handle_INSTR_A2MBCL.ok
				call	os_print_string
				call	os_print_newline
				jmp	OpcodeGenerator.execute.nextInstruction
			.error:
				jmp	OpcodeGenerator.execute.invalidArgument

			msg.handle_INSTR_A2MBCL 	db "          ---> Arg2 must be CL ... ", 0
			msg.handle_INSTR_A2MBCL.ok	db "ok", 0

	; private static methods (debug functions)
	; ----------------------------------------------------------------------

		private.OpcodeGenerator.debug.printIndex:
		; --------------------------------------------------------------
				mov	si, msg.indexEqual
				call	os_print_string
				mov	ax, [OpcodeGenerator.index]
				call	os_int_to_string
				mov	si, ax
				call	os_print_string
				call	os_print_newline
				ret
			msg.indexEqual	db "          Offset=", 0
			msg.index	dw 0, 0

		private.OpcodeGenerator.debug.printHexByte:
		; --------------------------------------------------------------
		; AX = value
				mov	si, msg.axEqual
				call	os_print_string
				call	os_print_2hex
				ret
			msg.axEqual	db "", 0

		private.OpcodeGenerator.debug.printHexWord:
		; --------------------------------------------------------------
		; AL = value
				mov	si, msg.alEqual
				call	os_print_string
				call	os_print_4hex
				ret
			msg.alEqual	db "", 0

		msg.debug.here	db "--> here <--", 0
;===============================================================================

