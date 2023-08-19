[bits 32]
section .text
global _start
%include "aal/libaal.asm"
; ///
; comment: To hex converter
; comment: Argument : number
; ///
; func entry(dword argc pointer prog_name pointer number_str)
; func entry:
entry:
	push ebp
	mov ebp, esp

	; [
	; dword n
	; byte str<16>
	; ]
	sub dword esp, 20

	; test [argc < 2]
	mov dword eax, [ebp+8]
	mov dword ebx, 2
	cmp eax, ebx
	jae entry_fail_condition0

		; print_str("Error : Usage : ")
		; argument "Error : Usage : "
		push data1
		call print_str
		add esp, 4

		; print_str([prog_name])
		; argument [prog_name]
		mov dword eax, [ebp+12]
		push eax
		call print_str
		add esp, 4

		; print_str(" <number>\n")
		; argument " <number>\n"
		push data2
		call print_str
		add esp, 4

		; fail
		jmp entry_end_condition0
		entry_fail_condition0:
			; n = [string_to_dword([number_str])]
			; argument [number_str]
			mov dword eax, [ebp+16]
			push eax
			call string_to_dword
			add esp, 4

			mov dword [ebp-4], eax

			; print_dword_hex([n])
			; argument [n]
			mov dword eax, [ebp-4]
			push eax
			call print_dword_hex
			add esp, 4

			; print_str("\n")
			; argument "\n"
			push data3
			call print_str
			add esp, 4

			; end
	entry_end_condition0:
	leave
	ret

_start:
	call entry
	mov dword eax, 1
	mov dword ebx, 0
	int 0x80

section .data
data0 db "0x", 0x00
data1 db "Error : Usage : ", 0x00
data2 db " <number>", 0xa, "", 0x00
data3 db "", 0xa, "", 0x00
