;===============================================================================
; class MemTest
;===============================================================================
	bits	16
	org	32768

	; ----------------------------------------------------------------------
	test1:
		call	os_print_newline
		call	Mem.clear
	
		; db 164, 203, 221, 203, 163, 0, 2, 10000010b ; [ SI + imm16 + BP ]
		mov	al, 164
		call	Mem.addTokenId
		mov	al, 203
		call	Mem.addTokenId
		mov	al, 221
		call	Mem.addTokenId
		mov	al, 203
		call	Mem.addTokenId
		mov	al, 163
		call	Mem.addTokenId

		call	Mem.classify
		cmp	ah, 0
		je	.notValid
	.ok:
		mov	si, msg.mem.classify.ok
		call	os_print_string
		call	os_print_newline

		mov	si, msg.mem.valueEqual
		call	os_print_string
		mov	si, Mem.value
		call	os_print_string
		call	os_print_newline

		mov	si, msg.mem.indexEqual
		call	os_print_string
		mov	ah, 0
		mov	al, [Mem.index]
		call	os_int_to_string
		mov	si, ax
		call	os_print_string
		call	os_print_newline

		mov	si, msg.mem.rmEqual
		call	os_print_string
		mov	ah, 0
		mov	al, [Mem.rm]
		call	os_int_to_string
		mov	si, ax
		call	os_print_string
		call	os_print_newline

		mov	si, msg.mem.isIbOrIwEqual
		call	os_print_string
		mov	ah, 0
		mov	al, [Mem.isIbOrIw]
		call	os_int_to_string
		mov	si, ax
		call	os_print_string
		call	os_print_newline

		call	test2 ; <-----------------

		ret

	.notValid:
		mov	si, msg.mem.classify.notValid
		call	os_print_string
		call	os_print_newline

		; call	os_wait_for_key
		ret

	; ----------------------------------------------------------------------
	test2:
		call	os_print_newline
		call	Mem.clear
	
		; db 165, 203, 220,   0,   0, 0, 1, 01000101b ; [ DI + imm8 ]
		mov	al, 165
		call	Mem.addTokenId
		mov	al, 203
		call	Mem.addTokenId
		mov	al, 220
		call	Mem.addTokenId

		call	Mem.classify
		cmp	ah, 0
		je	.notValid
	.ok:
		mov	si, msg.mem.classify.ok
		call	os_print_string
		call	os_print_newline

		mov	si, msg.mem.valueEqual
		call	os_print_string
		mov	si, Mem.value
		call	os_print_string
		call	os_print_newline

		mov	si, msg.mem.indexEqual
		call	os_print_string
		mov	ah, 0
		mov	al, [Mem.index]
		call	os_int_to_string
		mov	si, ax
		call	os_print_string
		call	os_print_newline

		mov	si, msg.mem.rmEqual
		call	os_print_string
		mov	ah, 0
		mov	al, [Mem.rm]
		call	os_int_to_string
		mov	si, ax
		call	os_print_string
		call	os_print_newline

		mov	si, msg.mem.isIbOrIwEqual
		call	os_print_string
		mov	ah, 0
		mov	al, [Mem.isIbOrIw]
		call	os_int_to_string
		mov	si, ax
		call	os_print_string
		call	os_print_newline

		ret

	.notValid:
		mov	si, msg.mem.classify.notValid
		call	os_print_string
		call	os_print_newline

		; call	os_wait_for_key
		ret

	; ----------------------------------------------------------------------
		; call	os_wait_for_key
		ret

	; ----------------------------------------------------------------------

	%include	"../../include/mikedev.inc"
	%include	"../../include/Mem.inc"

	msg.mem.classify.ok		db "Classify ok ! ", 0
	msg.mem.classify.notValid	db "Classify not valid ! ", 0

	msg.mem.valueEqual		db "Value = ", 0
	msg.mem.indexEqual		db "Index = ", 0
	msg.mem.rmEqual			db "R/M = ", 0
	msg.mem.isIbOrIwEqual		db "ib or iw = ", 0

;===============================================================================
