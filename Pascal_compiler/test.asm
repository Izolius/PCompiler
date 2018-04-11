format PE console
entry main
include 'win32a.inc'
section '.text' code executable

	main:
	push ebp
	mov ebp, esp
	;add to stack a
	sub esp, 4
	;add to stack b
	sub esp, 4
	;read addr of a{
		mov eax, ebp
		sub eax, 4
	;}
	push eax
	;read addr of b{
		mov eax, ebp
		sub eax, 8
	;}
	push eax
	pop eax
	push dword[eax]
	; assign {
		pop eax
		pop ecx
		mov [ecx], eax
	;}
	; free local variables
	add esp, 8
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