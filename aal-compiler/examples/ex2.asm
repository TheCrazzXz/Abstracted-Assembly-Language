[bits 32]
section .text
global _start
%include "aal/libaal.asm"
; func is_prime(dword num)
; func is_prime:
is_prime:
	push ebp
	mov ebp, esp

	; {
	; [
	; dword i
	; ]
	sub dword esp, 4

	; test [num < 2]
	mov dword eax, [ebp+8]
	mov dword ebx, 2
	cmp eax, ebx
	jae is_prime_fail_condition0

		; {
		; return [0]
		xor dword eax, eax
		; leave function
		leave
		ret

; }
; i = 2
is_prime_fail_condition0:
is_prime_end_condition0:
mov dword [ebp-4], 2

; // loop for all numbers in [0 to num[
; do
jmp is_prime_loop_test0
is_prime_loop0:
; {
; test [num % i == 0]
mov dword eax, [ebp+8]
mov dword ebx, [ebp-4]
xor edx, edx
div ebx
mov dword eax, edx
xor dword ebx, ebx
cmp eax, ebx
jne is_prime_fail_condition1

	; return [0]
	xor dword eax, eax
	; leave function
	leave
	ret

; end
is_prime_fail_condition1:
is_prime_end_condition1:
; i+
mov eax, [ebp-4]
inc eax
mov dword [ebp-4], eax

; }
; loop [i < num]
is_prime_loop_test0:
mov dword eax, [ebp-4]
mov dword ebx, [ebp+8]
cmp eax, ebx
jb is_prime_loop0

is_prime_loop_end0:
; return [1]
mov dword eax, 1
; leave function
leave
ret

; }
leave
ret

; // get the number of prime numbers between [min and max]
; func get_nprime_range(dword min dword max)
; func get_nprime_range:
get_nprime_range:
	push ebp
	mov ebp, esp

	; {
	; [
	; dword nprimes
	; ]
	sub dword esp, 4

	; nprimes = 0
	mov dword [ebp-4], 0

	; do
	jmp get_nprime_range_loop_test1
	get_nprime_range_loop1:
	; {
	; test [is_prime([max])]
	; argument [max]
	mov dword eax, [ebp+12]
	push eax
	call is_prime
	add esp, 4

	cmp eax, 0
	je get_nprime_range_fail_condition2

		; {
		; nprimes+
		mov eax, [ebp-4]
		inc eax
		mov dword [ebp-4], eax

		; }
		; max-
	get_nprime_range_fail_condition2:
	get_nprime_range_end_condition2:
	mov eax, [ebp+12]
	dec eax
	mov dword [ebp+12], eax

	; }
	; loop [max + 1 > min]
	get_nprime_range_loop_test1:
	mov dword eax, [ebp+12]
	add dword eax, 1
	mov dword ebx, [ebp+8]
	cmp eax, ebx
	ja get_nprime_range_loop1

	get_nprime_range_loop_end1:
	; return [nprimes]
	mov dword eax, [ebp-4]
	; leave function
	leave
	ret

; }
leave
ret

; func entry()
; func entry:
entry:
	push ebp
	mov ebp, esp

	; {
	; [
	; dword min
	; dword max
	; dword nprimes
	; ]
	sub dword esp, 12

	; print_str("PRIME RANGE SEARCHER 3000\nThis program will search with a very fast algorithm how many prime numbers (integers that can't be divided except by 1 or by themselves) in a certain range\n")
	; argument "PRIME RANGE SEARCHER 3000\nThis program will search with a very fast algorithm how many prime numbers (integers that can't be divided except by 1 or by themselves) in a certain range\n"
	push data1
	call print_str
	add esp, 4

	; print_str("Enter min number range : ")
	; argument "Enter min number range : "
	push data2
	call print_str
	add esp, 4

	; min = [read_dword()]
	call read_dword
	mov dword [ebp-4], eax

	; print_str("Enter max number range : ")
	; argument "Enter max number range : "
	push data3
	call print_str
	add esp, 4

	; max = [read_dword()]
	call read_dword
	mov dword [ebp-8], eax

	; nprimes = [get_nprime_range([min] [max])]
	; argument [max]
	mov dword eax, [ebp-8]
	push eax
	; argument [min]
	mov dword eax, [ebp-4]
	push eax
	call get_nprime_range
	add esp, 8

	mov dword [ebp-12], eax

	; print_str("There are ")
	; argument "There are "
	push data4
	call print_str
	add esp, 4

	; print_dword([nprimes])
	; argument [nprimes]
	mov dword eax, [ebp-12]
	push eax
	call print_dword
	add esp, 4

	; print_str(" prime numbers in range [")
	; argument " prime numbers in range ["
	push data5
	call print_str
	add esp, 4

	; print_dword([min])
	; argument [min]
	mov dword eax, [ebp-4]
	push eax
	call print_dword
	add esp, 4

	; print_str(" - ")
	; argument " - "
	push data6
	call print_str
	add esp, 4

	; print_dword([max])
	; argument [max]
	mov dword eax, [ebp-8]
	push eax
	call print_dword
	add esp, 4

	; print_str("]\n")
	; argument "]\n"
	push data7
	call print_str
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
data1 db "PRIME RANGE SEARCHER 3000", 0xa, "This program will search with a very fast algorithm how many prime numbers (integers that can't be divided except by 1 or by themselves) in a certain range", 0xa, "", 0x00
data2 db "Enter min number range : ", 0x00
data3 db "Enter max number range : ", 0x00
data4 db "There are ", 0x00
data5 db " prime numbers in range [", 0x00
data6 db " - ", 0x00
data7 db "]", 0xa, "", 0x00
