format PE console
entry main
include 'D:\Projects\Visual_Studio\Pascal_compiler\FASM\win32a.inc'
section '.text' code executable

	main:
	push ebp
	mov ebp, esp
	;add to stack arr
	sub esp, 16
	;add to stack i
	sub esp, 4
	;add to stack j
	sub esp, 4
	;add to stack ki
	sub esp, 4
	;add to stack k
	sub esp, 4
		;read addr of k{
			mov eax, ebp
			sub eax, 32
		;}
		push eax
		;push const to stack
		push dword 97
		; assign {
			pop eax
			pop ecx
			mov [ecx], eax
		;}
		;push const to stack
		push dword 99
		__forstart1:
		;read k from stack {
			mov eax, ebp
			sub eax, 32
		;}
		pop ecx
		clc
		cmp dword[eax], ecx
		push ecx
		jg __forend1
		;read addr of arr{
			mov eax, ebp
			sub eax, 16
		;}
		push eax
		pop eax
		;push const to stack
		push dword 0
		;push const to stack
		push dword 0
		;eval indexed var addr{
			mov ecx, 0
			pop edx
			sub edx, 1
			imul edx, 12
			add ecx, edx
			pop edx
			sub edx, 1
			imul edx, 2
			imul edx, 12
			add ecx, edx
			add eax, ecx
		;}
		push eax
		pop eax
		;read addr of k{
			mov ecx, ebp
			sub ecx, 32
		;}
		push ecx
		pop ecx
		push dword[ecx]
		;eval indexed var addr{
			mov ecx, 0
			pop edx
			sub edx, 97
			imul edx, 4
			add ecx, edx
			add eax, ecx
		;}
		push eax
		;push const to stack
		push dword 13
		; assign {
			pop eax
			pop ecx
			mov [ecx], eax
		;}
		;read addr of k{
			mov eax, ebp
			sub eax, 32
		;}
		inc dword[eax]
		jmp __forstart1
		__forend1:
		;read addr of k{
			mov eax, ebp
			sub eax, 32
		;}
		push eax
		;push const to stack
		push dword 97
		; assign {
			pop eax
			pop ecx
			mov [ecx], eax
		;}
		;push const to stack
		push dword 99
		__forstart2:
		;read k from stack {
			mov eax, ebp
			sub eax, 32
		;}
		pop ecx
		clc
		cmp dword[eax], ecx
		push ecx
		jg __forend2
		;read addr of arr{
			mov eax, ebp
			sub eax, 16
		;}
		push eax
		pop eax
		;push const to stack
		push dword 0
		;push const to stack
		push dword 0
		;eval indexed var addr{
			mov ecx, 0
			pop edx
			sub edx, 1
			imul edx, 12
			add ecx, edx
			pop edx
			sub edx, 1
			imul edx, 2
			imul edx, 12
			add ecx, edx
			add eax, ecx
		;}
		push eax
		pop eax
		;read addr of k{
			mov ecx, ebp
			sub ecx, 32
		;}
		push ecx
		pop ecx
		push dword[ecx]
		;eval indexed var addr{
			mov ecx, 0
			pop edx
			sub edx, 97
			imul edx, 4
			add ecx, edx
			add eax, ecx
		;}
		push eax
		pop eax
		push dword[eax]
		call printi
		; free local variables
		add esp, 4
		;read addr of k{
			mov eax, ebp
			sub eax, 32
		;}
		inc dword[eax]
		jmp __forstart2
		__forend2:
	;read addr of arr{
		mov eax, ebp
		sub eax, 16
	;}
	push eax
	pop eax
	;push const to stack
	push dword 0
	;push const to stack
	push dword 0
	;eval indexed var addr{
		mov ecx, 0
		pop edx
		sub edx, 1
		imul edx, 12
		add ecx, edx
		pop edx
		sub edx, 1
		imul edx, 2
		imul edx, 12
		add ecx, edx
		add eax, ecx
	;}
	push eax
	pop eax
	;push const to stack
	push dword 99
	;eval indexed var addr{
		mov ecx, 0
		pop edx
		sub edx, 97
		imul edx, 4
		add ecx, edx
		add eax, ecx
	;}
	push eax
	;push const to stack
	push dword 13
	; assign {
		pop eax
		pop ecx
		mov [ecx], eax
	;}
	;read addr of arr{
		mov eax, ebp
		sub eax, 16
	;}
	push eax
	pop eax
	;push const to stack
	push dword 0
	;push const to stack
	push dword 0
	;eval indexed var addr{
		mov ecx, 0
		pop edx
		sub edx, 1
		imul edx, 12
		add ecx, edx
		pop edx
		sub edx, 1
		imul edx, 2
		imul edx, 12
		add ecx, edx
		add eax, ecx
	;}
	push eax
	pop eax
	;push const to stack
	push dword 99
	;eval indexed var addr{
		mov ecx, 0
		pop edx
		sub edx, 97
		imul edx, 4
		add ecx, edx
		add eax, ecx
	;}
	push eax
	pop eax
	push dword[eax]
	call printi
	; free local variables
	add esp, 4
	; free local variables
	add esp, 20
	pop ebp
	call [ExitProcess]

printc:
	push dword [esp+4]
    push __cmsg__
    call [printf]
	add esp, 8
    ret
printi:
	push dword [esp+4]
	push __imsg__
	call [printf]
	add esp, 8
	ret
printr:
	push dword [esp+4]
    push __fmsg__
    call [printf]
	add esp, 4
    ret
section '.rdata' data readable
    __imsg__ db '%d',10, 0   
	__fmsg__ db '%f',10, 0   
	__cmsg__ db '%c',10, 0
section '.idata' data readable import
	library kernel32, 'kernel32.dll',\
		msvcrt, 'msvcrt.dll'
	import kernel32, ExitProcess, 'ExitProcess'
	import msvcrt, printf, 'printf'