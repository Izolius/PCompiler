printi:
	push dword [esp+4]
	push __imsg__
	call [printf]
	add esp, 8
	ret