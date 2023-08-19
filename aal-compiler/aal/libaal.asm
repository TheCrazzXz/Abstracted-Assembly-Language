	; // reverse the string str in place
	; func reverse_string(pointer str dword len)
	; func reverse_string:
reverse_string:
	push ebp
	mov ebp, esp

	; {
	; [
	; byte c
	; pointer left
	; pointer right
	sub dword esp, 9

	; ]
	; left = [str]
	mov dword eax, [ebp+8]
	mov dword [ebp-5], eax

	; right = [str + len - 1]
	mov dword eax, [ebp+8]
	add dword eax, [ebp+12]
	sub dword eax, 1
	mov dword [ebp-9], eax

	; do
	jmp reverse_string_loop_test0
	reverse_string_loop0:
	; {
	; c = [b$right]
	mov ebx, [ebp-9]
	mov byte al, [ebx]
	mov byte [ebp-1], al

	; b$right = [b$left]
	mov ebx, [ebp-5]
	mov byte al, [ebx]
	mov ebx, [ebp-9]
	mov byte [ebx], al

	; b$left = [c]
	mov byte al, [ebp-1]
	mov ebx, [ebp-5]
	mov byte [ebx], al

	; right-
	mov eax, [ebp-9]
	dec eax
	mov dword [ebp-9], eax

	; left+
	mov eax, [ebp-5]
	inc eax
	mov dword [ebp-5], eax

	; }
	; loop [left < right]
	reverse_string_loop_test0:
	mov dword eax, [ebp-5]
	mov dword ebx, [ebp-9]
	cmp eax, ebx
	jb reverse_string_loop0

	reverse_string_loop_end0:
	; }
	leave
	ret

; // converts unsigned dword (32-bit) val to string and store result in buf
; func dword_as_string(dword val pointer buf)
; func dword_as_string:
dword_as_string:
	push ebp
	mov ebp, esp

	; {
	; [
	; dword q
	; dword r
	; byte c
	; pointer str
	sub dword esp, 13

	; ]
	; str = [buf]
	mov dword eax, [ebp+12]
	mov dword [ebp-13], eax

	; q = [val]
	mov dword eax, [ebp+8]
	mov dword [ebp-4], eax

	; test [q == 0]
	mov dword eax, [ebp-4]
	xor dword ebx, ebx
	cmp eax, ebx
	jne dword_as_string_fail_condition0

		; {
		; b$buf = '0'
		mov eax, [ebp+12]
		mov byte [eax], 48

		; return [1]
		mov dword eax, 1
		; leave function
		leave
		ret

; }
; do
dword_as_string_fail_condition0:
dword_as_string_end_condition0:
jmp dword_as_string_loop_test1
dword_as_string_loop1:
; {
; r = [q % 10]
mov dword eax, [ebp-4]
mov dword ebx, 10
xor edx, edx
div ebx
mov dword eax, edx
mov dword [ebp-8], eax

; q = [q / 10]
mov dword eax, [ebp-4]
mov dword ebx, 10
xor edx, edx
div ebx
mov dword [ebp-4], eax

; c = [r + 48]
mov dword eax, [ebp-8]
add dword eax, 48
mov byte [ebp-9], al

; b$str = [c]
mov byte al, [ebp-9]
mov ebx, [ebp-13]
mov byte [ebx], al

; str+
mov eax, [ebp-13]
inc eax
mov dword [ebp-13], eax

; }
; loop [q != 0]
dword_as_string_loop_test1:
mov dword eax, [ebp-4]
xor dword ebx, ebx
cmp eax, ebx
jne dword_as_string_loop1

dword_as_string_loop_end1:
; reverse_string([buf] [str-buf])
; argument [str-buf]
mov dword eax, [ebp-13]
sub dword eax, [ebp+12]
push eax
; argument [buf]
mov dword eax, [ebp+12]
push eax
call reverse_string
add esp, 8

; return [str-buf]
mov dword eax, [ebp-13]
sub dword eax, [ebp+12]
; leave function
leave
ret

; }
leave
ret

; // converts unsigned dword (32-bit) val to string as hex and store result in buf
; func dword_as_string_hex(dword val pointer buf)
; func dword_as_string_hex:
dword_as_string_hex:
	push ebp
	mov ebp, esp

	; {
	; [
	; dword q
	; dword r
	; byte c
	; pointer str
	sub dword esp, 13

	; ]
	; str = [buf]
	mov dword eax, [ebp+12]
	mov dword [ebp-13], eax

	; q = [val]
	mov dword eax, [ebp+8]
	mov dword [ebp-4], eax

	; test [q == 0]
	mov dword eax, [ebp-4]
	xor dword ebx, ebx
	cmp eax, ebx
	jne dword_as_string_hex_fail_condition1

		; {
		; b$buf = '0'
		mov eax, [ebp+12]
		mov byte [eax], 48

		; return [1]
		mov dword eax, 1
		; leave function
		leave
		ret

; }
; do
dword_as_string_hex_fail_condition1:
dword_as_string_hex_end_condition1:
jmp dword_as_string_hex_loop_test2
dword_as_string_hex_loop2:
; {
; r = [q % 16]
mov dword eax, [ebp-4]
mov dword ebx, 16
xor edx, edx
div ebx
mov dword eax, edx
mov dword [ebp-8], eax

; q = [q / 16]
mov dword eax, [ebp-4]
shr eax, 4
mov dword [ebp-4], eax

; test [r < 10]
mov dword eax, [ebp-8]
mov dword ebx, 10
cmp eax, ebx
jae dword_as_string_hex_fail_condition2

	; c = [r + 48]
	mov dword eax, [ebp-8]
	add dword eax, 48
	mov byte [ebp-9], al

	; fail
	jmp dword_as_string_hex_end_condition2
	dword_as_string_hex_fail_condition2:
		; c = [r + 87]
		mov dword eax, [ebp-8]
		add dword eax, 87
		mov byte [ebp-9], al

		; end
dword_as_string_hex_end_condition2:
; b$str = [c]
mov byte al, [ebp-9]
mov ebx, [ebp-13]
mov byte [ebx], al

; str+
mov eax, [ebp-13]
inc eax
mov dword [ebp-13], eax

; }
; loop [q != 0]
dword_as_string_hex_loop_test2:
mov dword eax, [ebp-4]
xor dword ebx, ebx
cmp eax, ebx
jne dword_as_string_hex_loop2

dword_as_string_hex_loop_end2:
; reverse_string([buf] [str-buf])
; argument [str-buf]
mov dword eax, [ebp-13]
sub dword eax, [ebp+12]
push eax
; argument [buf]
mov dword eax, [ebp+12]
push eax
call reverse_string
add esp, 8

; return [str-buf]
mov dword eax, [ebp-13]
sub dword eax, [ebp+12]
; leave function
leave
ret

; }
leave
ret

; // get the len that make all the bytes from str until a null-byte
; func strlen_before_nullbyte(pointer str):
; func strlen_before_nullbyte:
strlen_before_nullbyte:
	push ebp
	mov ebp, esp

	; {
	; [
	; pointer buf
	sub dword esp, 4

	; ]
	; buf = [str]
	mov dword eax, [ebp+8]
	mov dword [ebp-4], eax

	; do
	jmp strlen_before_nullbyte_loop_test3
	strlen_before_nullbyte_loop3:
	; {
	; str+
	mov eax, [ebp+8]
	inc eax
	mov dword [ebp+8], eax

	; }
	; loop [b$str != 0]
	strlen_before_nullbyte_loop_test3:
	mov ebx, [ebp+8]
	mov byte al, [ebx]
	xor dword ebx, ebx
	cmp al, bl
	jne strlen_before_nullbyte_loop3

	strlen_before_nullbyte_loop_end3:
	; return [str-buf]
	mov dword eax, [ebp+8]
	sub dword eax, [ebp-4]
	; leave function
	leave
	ret

; }
leave
ret

; // print the unsigned dword val as integer value
; func print_dword(dword val)
; func print_dword:
print_dword:
	push ebp
	mov ebp, esp

	; {
	; [
	; dword len
	; byte str<16>
	sub dword esp, 20

	; ]
	; len = [dword_as_string([val] [&str])]
	; argument [&str]
	mov ebx, ebp
	sub ebx, 20
	mov dword eax, ebx
	push eax
	; argument [val]
	mov dword eax, [ebp+8]
	push eax
	call dword_as_string
	add esp, 8

	mov dword [ebp-4], eax

	; syscall SYS_WRITE STDOUT [&str] [len]
	; set eax
	mov dword eax, 4

	; set ebx
	mov dword ebx, 1

	; set ecx
	mov ecx, ebp
	sub ecx, 20
	mov dword ecx, ecx

	; set edx
	mov dword edx, [ebp-4]

	int 0x80
	; }
	leave
	ret

; func print_str(pointer str):
; func print_str:
print_str:
	push ebp
	mov ebp, esp

	; {
	; [
	; dword len
	sub dword esp, 4

	; ]
	; len = [strlen_before_nullbyte([str])]
	; argument [str]
	mov dword eax, [ebp+8]
	push eax
	call strlen_before_nullbyte
	add esp, 4

	mov dword [ebp-4], eax

	; syscall SYS_WRITE STDOUT [str] [len]
	; set eax
	mov dword eax, 4

	; set ebx
	mov dword ebx, 1

	; set ecx
	mov dword ecx, [ebp+8]

	; set edx
	mov dword edx, [ebp-4]

	int 0x80
	; }
	leave
	ret

; func print_dword_hex(dword val)
; func print_dword_hex:
print_dword_hex:
	push ebp
	mov ebp, esp

	; {
	; [
	; dword len
	; byte str<16>
	sub dword esp, 20

	; ]
	; len = [dword_as_string_hex([val] [&str])]
	; argument [&str]
	mov ebx, ebp
	sub ebx, 20
	mov dword eax, ebx
	push eax
	; argument [val]
	mov dword eax, [ebp+8]
	push eax
	call dword_as_string_hex
	add esp, 8

	mov dword [ebp-4], eax

	; print_str("0x")
	; argument "0x"
	push data0
	call print_str
	add esp, 4

	; syscall SYS_WRITE STDOUT [&str] [len]
	; set eax
	mov dword eax, 4

	; set ebx
	mov dword ebx, 1

	; set ecx
	mov ecx, ebp
	sub ecx, 20
	mov dword ecx, ecx

	; set edx
	mov dword edx, [ebp-4]

	int 0x80
	; }
	leave
	ret

; func dword_pow(dword a dword exp)
; func dword_pow:
dword_pow:
	push ebp
	mov ebp, esp

	; {
	; [
	; dword result
	; dword i
	sub dword esp, 8

	; ]
	; result = 1
	mov dword [ebp-4], 1

	; i = 0
	mov dword [ebp-8], 0

	; do
	jmp dword_pow_loop_test4
	dword_pow_loop4:
	; {
	; result = [result * a]
	mov dword eax, [ebp-4]
	mov dword ebx, [ebp+8]
	mul ebx
	mov dword [ebp-4], eax

	; i+
	mov eax, [ebp-8]
	inc eax
	mov dword [ebp-8], eax

	; }
	; loop [i < exp]
	dword_pow_loop_test4:
	mov dword eax, [ebp-8]
	mov dword ebx, [ebp+12]
	cmp eax, ebx
	jb dword_pow_loop4

	dword_pow_loop_end4:
	; // print_dword([result])
	; return [result]
	mov dword eax, [ebp-4]
	; leave function
	leave
	ret

; }
leave
ret

; // print the null-terminated string str
; func string_to_dword(pointer str)
; func string_to_dword:
string_to_dword:
	push ebp
	mov ebp, esp

	; {
	; [
	; dword result
	; dword len
	; dword digit
	; dword pow
	; dword i
	sub dword esp, 20

	; ]
	; i = 0,
	mov dword [ebp-20], 0

	; result = 0
	mov dword [ebp-4], 0

	; len = [strlen_before_nullbyte([str])]
	; argument [str]
	mov dword eax, [ebp+8]
	push eax
	call strlen_before_nullbyte
	add esp, 4

	mov dword [ebp-8], eax

	; do
	jmp string_to_dword_loop_test5
	string_to_dword_loop5:
	; {
	; digit = [b$str - 48]
	xor eax, eax
	mov ebx, [ebp+8]
	mov byte al, [ebx]
	sub dword eax, 48
	mov dword [ebp-12], eax

	; pow = [dword_pow(10 [len-i-1])]
	; argument [len-i-1]
	mov dword eax, [ebp-8]
	sub dword eax, [ebp-20]
	sub dword eax, 1
	push eax
	; argument 10
	push 10
	call dword_pow
	add esp, 8

	mov dword [ebp-16], eax

	; digit = [digit * pow]
	mov dword eax, [ebp-12]
	mov dword ebx, [ebp-16]
	mul ebx
	mov dword [ebp-12], eax

	; result = [result + digit]
	mov dword eax, [ebp-4]
	add dword eax, [ebp-12]
	mov dword [ebp-4], eax

	; str+
	mov eax, [ebp+8]
	inc eax
	mov dword [ebp+8], eax

	; i+
	mov eax, [ebp-20]
	inc eax
	mov dword [ebp-20], eax

	; }
	; loop [i < len]
	string_to_dword_loop_test5:
	mov dword eax, [ebp-20]
	mov dword ebx, [ebp-8]
	cmp eax, ebx
	jb string_to_dword_loop5

	string_to_dword_loop_end5:
	; return [result]
	mov dword eax, [ebp-4]
	; leave function
	leave
	ret

; }
leave
ret

; // copy len bytes from src to dst
; func copybuf(pointer dst pointer src dword len)
; func copybuf:
copybuf:
	push ebp
	mov ebp, esp

	; {
	; do
	jmp copybuf_loop_test6
	copybuf_loop6:
	; {
	; b$dst = [b$src]
	mov ebx, [ebp+12]
	mov byte al, [ebx]
	mov ebx, [ebp+8]
	mov byte [ebx], al

	; src+
	mov eax, [ebp+12]
	inc eax
	mov dword [ebp+12], eax

	; dst+
	mov eax, [ebp+8]
	inc eax
	mov dword [ebp+8], eax

	; len-
	mov eax, [ebp+16]
	dec eax
	mov dword [ebp+16], eax

	; }
	; loop [len]
	copybuf_loop_test6:
	mov dword eax, [ebp+16]
	cmp eax, 0
	jne copybuf_loop6

	copybuf_loop_end6:
	; }
	leave
	ret

; // copy dst to src
; func copystring(pointer dst pointer src)
; func copystring:
copystring:
	push ebp
	mov ebp, esp

	; {
	; [
	; dword len
	sub dword esp, 4

	; ]
	; len = [strlen_before_nullbyte([src])]
	; argument [src]
	mov dword eax, [ebp+12]
	push eax
	call strlen_before_nullbyte
	add esp, 4

	mov dword [ebp-4], eax

	; copybuf([dst] [src] [len])
	; argument [len]
	mov dword eax, [ebp-4]
	push eax
	; argument [src]
	mov dword eax, [ebp+12]
	push eax
	; argument [dst]
	mov dword eax, [ebp+8]
	push eax
	call copybuf
	add esp, 12

	; }
	leave
	ret

; // compare len bytes from mema and memb 
; // returns 1 if equal, returns 2 if not equal
; func memequal(pointer mema pointer memb dword len)
; func memequal:
memequal:
	push ebp
	mov ebp, esp

	; {
	; do
	jmp memequal_loop_test7
	memequal_loop7:
	; {
	; test [b$mema == b$memb]
	mov ebx, [ebp+8]
	mov byte al, [ebx]
	mov ecx, [ebp+12]
	mov byte bl, [ecx]
	cmp al, bl
	jne memequal_fail_condition3

		; fail
		jmp memequal_end_condition3
		memequal_fail_condition3:
			; return [0]
			xor dword eax, eax
			; leave function
			leave
			ret

; end
memequal_end_condition3:
; len-
mov eax, [ebp+16]
dec eax
mov dword [ebp+16], eax

; }
; loop [len]
memequal_loop_test7:
mov dword eax, [ebp+16]
cmp eax, 0
jne memequal_loop7

memequal_loop_end7:
; return [1]
mov dword eax, 1
; leave function
leave
ret

; }
leave
ret

; // compare stra and strb, are they equal
; func strequal(pointer stra pointer strb)
; func strequal:
strequal:
	push ebp
	mov ebp, esp

	; {
	; [
	; dword stra_len
	; dword strb_len
	sub dword esp, 8

	; ]
	; stra_len = [strlen_before_nullbyte([stra])]
	; argument [stra]
	mov dword eax, [ebp+8]
	push eax
	call strlen_before_nullbyte
	add esp, 4

	mov dword [ebp-4], eax

	; strb_len = [strlen_before_nullbyte([strb])]
	; argument [strb]
	mov dword eax, [ebp+12]
	push eax
	call strlen_before_nullbyte
	add esp, 4

	mov dword [ebp-8], eax

	; test [stra_len != strb_len]
	mov dword eax, [ebp-4]
	mov dword ebx, [ebp-8]
	cmp eax, ebx
	je strequal_fail_condition4

		; {
		; return [0]
		xor dword eax, eax
		; leave function
		leave
		ret

; }
; return [memequal([stra] [strb] [strb_len])]
strequal_fail_condition4:
strequal_end_condition4:
; argument [strb_len]
mov dword eax, [ebp-8]
push eax
; argument [strb]
mov dword eax, [ebp+12]
push eax
; argument [stra]
mov dword eax, [ebp+8]
push eax
call memequal
add esp, 12

; leave function
leave
ret

; }
leave
ret

; func read_str(pointer dst)
; func read_str:
read_str:
	push ebp
	mov ebp, esp

	; {
	; [
	; byte c
	sub dword esp, 1

	; ]
	; do
	jmp read_str_loop_test8
	read_str_loop8:
	; {
	; // read one character from stdin
	; syscall SYS_READ STDIN [&c] 1
	; set eax
	mov dword eax, 3

	; set ebx
	mov dword ebx, 0

	; set ecx
	mov ecx, ebp
	sub ecx, 1
	mov dword ecx, ecx

	; set edx
	mov dword edx, 1

	int 0x80
	; test [c == 10]
	mov byte al, [ebp-1]
	mov dword ebx, 10
	cmp al, bl
	jne read_str_fail_condition5

		; {
		; b$dst = 0
		mov eax, [ebp+8]
		mov byte [eax], 0

		; return
		; leave function
		leave
		ret

; }
; b$dst = [c]
read_str_fail_condition5:
read_str_end_condition5:
mov byte al, [ebp-1]
mov ebx, [ebp+8]
mov byte [ebx], al

; dst+
mov eax, [ebp+8]
inc eax
mov dword [ebp+8], eax

; }
; loop [1]
read_str_loop_test8:
mov dword eax, 1
cmp eax, 0
jne read_str_loop8

read_str_loop_end8:
; }
leave
ret

; func read_dword()
; func read_dword:
read_dword:
	push ebp
	mov ebp, esp

	; {
	; [
	; byte str<16>
	sub dword esp, 16

	; ]
	; read_str([&str])
	; argument [&str]
	mov ebx, ebp
	sub ebx, 16
	mov dword eax, ebx
	push eax
	call read_str
	add esp, 4

	; return [string_to_dword([&str])]
	; argument [&str]
	mov ebx, ebp
	sub ebx, 16
	mov dword eax, ebx
	push eax
	call string_to_dword
	add esp, 4

	; leave function
	leave
	ret

; }
leave
ret

