;===============================================================================
; class TokenTest
;===============================================================================
		bits	16
		org	32768
start:
	; ----------------------------------------------------------------------
		mov	si, msg.characterToVerify
		call	printString
		mov	al, [character] ; al = ASCII char code to verify

	; ----------------------------------------------------------------------
	verifyCharacterIsValid:
			call	Character.isValid
			cmp	ah, 1
			jz	verifyCharacterIsValid.true
			mov	si, msg.isValid.false
			jmp	verifyCharacterIsValid.continue
		verifyCharacterIsValid.true:
			mov	si, msg.isValid.true
		verifyCharacterIsValid.continue:
			call	printString

	; ----------------------------------------------------------------------
	verifyCharacterIsDigit:
			call	Character.isDigit
			cmp	ah, 1
			jz	verifyCharacterIsDigit.true
			mov	si, msg.isDigit.false
			jmp	verifyCharacterIsDigit.continue
		verifyCharacterIsDigit.true:
			mov	si, msg.isDigit.true
		verifyCharacterIsDigit.continue:
			call	printString

	; ----------------------------------------------------------------------
	verifyCharacterIsLetter:
			call	Character.isLetter
			cmp	ah, 1
			jz	verifyCharacterIsLetter.true
			mov	si, msg.isLetter.false
			jmp	verifyCharacterIsLetter.continue
		verifyCharacterIsLetter.true:
			mov	si, msg.isLetter.true
		verifyCharacterIsLetter.continue:
			call	printString

	; ----------------------------------------------------------------------
	characterConvertToUpper:
			call	Character.toUpper
			mov	byte [characterToUpper], al
			mov	si, characterToUpper
			call	printString

	; ----------------------------------------------------------------------
	TokenTest.end:
			ret

	; ----------------------------------------------------------------------
	printString:
		call	os_print_string
		call	os_print_newline
		; call	os_wait_for_key
		ret

	; ----------------------------------------------------------------------
	%include	"../../include/mikedev.inc"
	%include	"../../include/Character.inc"

	msg.characterToVerify	db "CHARACTER TO VERIFY: "

	character		db '+', 0

	msg.isValid.true	db "IS VALID !", 0
	msg.isValid.false	db "NOT IS VALID !", 0

	msg.isDigit.true	db "IS DIGIT !", 0
	msg.isDigit.false	db "NOT IS DIGIT !", 0

	msg.isLetter.true	db "IS LETTER !", 0
	msg.isLetter.false	db "NOT IS LETTER !", 0

	msg.isLetter.toUpper	db "CHARACTER TO UPPER: "
	characterToUpper	db '_', 0

;===============================================================================
