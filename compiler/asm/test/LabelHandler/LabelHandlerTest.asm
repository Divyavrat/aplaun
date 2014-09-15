;===============================================================================
; class LabelHandlerTest
;===============================================================================
		bits	16
		org	32768
start:
	; ----------------------------------------------------------------------
		mov	si, msg.testLabel
		call	printString

	; ----------------------------------------------------------------------
		mov	ax, labelName
		mov	dx, [labelOffset]
		call	LabelHandler.add

		mov	si, labelName
		call	printString
		mov	si, [LabelHandler.startMemoryAddress]
		call	printString

		mov	bx, [LabelHandler.startMemoryAddress]

		mov	ax, [bx+14]
		call	os_int_to_string
		mov	si, ax
		call	printString


	; ----------------------------------------------------------------------
		mov	ax, labelName2
		mov	dx, [labelOffset2]
		call	LabelHandler.add

		mov	si, labelName
		call	printString
		mov	si, [LabelHandler.startMemoryAddress]
		add	si, 16
		call	printString

		mov	bx, [LabelHandler.startMemoryAddress]
		mov	ax, [bx+14+16]
		call	os_int_to_string
		mov	si, ax
		call	printString

	; ----------------------------------------------------------------------
		mov	ax, labelName3
		mov	dx, [labelOffset3]
		call	LabelHandler.add

		mov	si, labelName
		call	printString
		mov	si, [LabelHandler.startMemoryAddress]
		add	si, 32
		call	printString

		mov	bx, [LabelHandler.startMemoryAddress]

		mov	ax, [bx+14+32]
		call	os_int_to_string
		mov	si, ax
		call	printString


	; ----------------------------------------------------------------------
		mov	ax, labelToSearch
		call	LabelHandler.getOffset ; return offset of label in DX
		cmp	bl, 1
		jne	.notFound
	.found:
		mov	si, msg.found
		call	printString
		mov	si, msg.dxEqual
		call	printString
	
		mov	ax, dx
		call	os_int_to_string
		mov	si, ax
		call	printString

		jmp	.end
	.notFound:
		mov	si, msg.notFound
		call	printString
		jmp	.end

	.end
		ret

	; ----------------------------------------------------------------------
	printString:
		call	os_print_string
		call	os_print_newline
		; call	os_wait_for_key
		ret

	; ----------------------------------------------------------------------
	%include	"../../include/mikedev.inc"
	%include	"../../include/LabelHandler.inc"

	msg.testLabel		db "Test label", 0
	msg.found		db "Found !", 0
	msg.notFound		db "Not found !", 0
	msg.dxEqual		db "DX=", 0

	labelName		db "myLabel", 0
	labelOffset		dw 1234

	labelName2		db "abc", 0
	labelOffset2		dw 5678

	labelName3		db "l234567890123", 0
	labelOffset3		dw 33333

	labelToSearch		db "l234567890123", 0

;===============================================================================
