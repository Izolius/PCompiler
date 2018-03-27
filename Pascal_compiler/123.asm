format PE console
entry main
include 'win32a.inc'
section '.text' code executable

	main:
	mov ebp, esp
	push ebp
	;add to stack a
	add esp, 4
	;add to stack c
	add esp, 4
	;push const to stack
	push dword 1
	;push const to stack
	push dword 2
	call myproc
	; free local variables
	sub esp, 8
	pop ebp
	call [ExitProcess]

	myproc:
	mov ebp, esp
	push ebp
	;add to stack d
	add esp, 4
	;read addr of d{
	mov eax, ebp
	sub eax, 8
	;}
	push eax
	;read addr of b{
	mov eax, ebp
	sub eax, 0
	;}
	push eax
	pop eax
	push dword[eax]
	pop eax
	pop ecx
	mov [ecx], eax
	;read addr of a{
	mov eax, ebp
	sub eax, 4
	;}
	push eax
	;read addr of c{
	mov eax, ebp
	mov eax, [eax-4]
	sub eax, 12
	;}
	push eax
	pop eax
	push dword[eax]
	pop eax
	pop ecx
	mov [ecx], eax
	; free local variables
	sub esp, 4
	pop ebp
	; free local variables
	sub esp, 8
	ret

section '.rdata' data readable
	__msg__ db '%x', 0
section '.idata' data readable import
	library kernel32, 'kernel32.dll',\
		msvcrt, 'msvcrt.dll'
	import kernel32, ExitProcess, 'ExitProcess'
	import msvcrt, printf, 'printf'