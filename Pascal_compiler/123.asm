format PE console
entry main
include 'win32a.inc'
section '.text' code executable

	main:
	mov ebp, esp
	repeat 4
		dec esp
	end repeat
	push dword 0
	pop eax
	mov [ebp-4], eax
	push dword 1
	push dword 2
	push dword 3
	pop eax
	pop ecx
	imul ecx, eax
	push ecx
	pop eax
	pop ecx
	add ecx, eax
	push ecx
	mov eax, [ebp-4]
	push eax
	pop eax
	pop ecx
	add ecx, eax
	push ecx
	pop eax
	mov [ebp-4], eax
	call [ExitProcess]


section '.idata' data readable import
library kernel32, 'kernel32.dll',\
msvcrt, 'msvcrt.dll'
import kernel32, ExitProcess, 'ExitProcess'
import msvcrt, printf, 'printf'