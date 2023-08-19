[bits 32]
section .text
global _start
%include "aal/libaal.asm"
; func entry(dword argc pointer program_name pointer pass)
; func entry:
entry:
	push ebp
	mov ebp, esp

	; {
	; [
	; dword password_correct
	; dword len
	; byte str<16>
	; ]
	sub dword esp, 24

	; copystring([&str] "password")
	; argument "password"
	push data1
	; argument [&str]
	mov ebx, ebp
	sub ebx, 24
	mov dword eax, ebx
	push eax
	call copystring
	add esp, 8

	; test [argc == 2]
	mov dword eax, [ebp+8]
	mov dword ebx, 2
	cmp eax, ebx
	jne entry_fail_condition0

		; {
		; password_correct = [strequal([pass] [&str])]
		; argument [&str]
		mov ebx, ebp
		sub ebx, 24
		mov dword eax, ebx
		push eax
		; argument [pass]
		mov dword eax, [ebp+16]
		push eax
		call strequal
		add esp, 8

		mov dword [ebp-4], eax

		; test [password_correct == 1]
		mov dword eax, [ebp-4]
		mov dword ebx, 1
		cmp eax, ebx
		jne entry_fail_condition1

			; {
			; print_str("Password is correct\n")
			; argument "Password is correct\n"
			push data2
			call print_str
			add esp, 4

			; }
			; fail
			jmp entry_end_condition1
			entry_fail_condition1:
				; {
				; print_str("Password is incorrect\n")
				; argument "Password is incorrect\n"
				push data3
				call print_str
				add esp, 4

				; }
		entry_end_condition1:
		; }
		; fail
		jmp entry_end_condition0
		entry_fail_condition0:
			; {
			; print_str("Error : Required one argument : password\n")
			; argument "Error : Required one argument : password\n"
			push data4
			call print_str
			add esp, 4

			; }
	entry_end_condition0:
	; }
	leave
	ret

_start:
	call entry
	mov dword eax, 1
	mov dword ebx, 0
	int 0x80

section .data
data0 db "0x", 0x00
data1 db "password", 0x00
data2 db "Password is correct", 0xa, "", 0x00
data3 db "Password is incorrect", 0xa, "", 0x00
data4 db "Error : Required one argument : password", 0xa, "", 0x00
