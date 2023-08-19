[bits 32]
section .text
global _start
%include "aal/libaal.asm"
; func entry(dword argc pointer prog_name pointer number_str)
; func entry:
entry:
	push ebp
	mov ebp, esp

	; {
	; pointer ptr
	; dword a
	; dword b
	; dword result
	sub dword esp, 16

	; a = 0
	mov dword [ebp-8], 0

	; b = 0
	mov dword [ebp-12], 0

	; // a = 0 and b = 0
	; result = [a+b]
	mov dword eax, [ebp-8]
	add dword eax, [ebp-12]
	mov dword [ebp-16], eax

	; print_dword([result])
	; argument [result]
	mov dword eax, [ebp-16]
	push eax
	call print_dword
	add esp, 4

	; // point ptr to the a
	; ptr = [&a]
	mov ebx, ebp
	sub ebx, 8
	mov dword eax, ebx
	mov dword [ebp-4], eax

	; // dereference the pointer as dword then get the value of the pointed dword variable, then a
	; result = [d$ptr]
	mov ebx, [ebp-4]
	mov dword eax, [ebx]
	mov dword [ebp-16], eax

	; // point ptr to the b
	; ptr = [&b]
	mov ebx, ebp
	sub ebx, 12
	mov dword eax, ebx
	mov dword [ebp-4], eax

	; // dereference the pointer as dword then get the value of the pointed dword variable, then b and set it to value
	; d$ptr = 589
	mov eax, [ebp-4]
	mov dword [eax], 589

	; // now b is set to 589
	; // a = 0 and b = 589, we've indirectly changed the value of b
	; result = [a+b]
	mov dword eax, [ebp-8]
	add dword eax, [ebp-12]
	mov dword [ebp-16], eax

	; print_dword([result])
	; argument [result]
	mov dword eax, [ebp-16]
	push eax
	call print_dword
	add esp, 4

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
