printc:
	push dword [esp+4]
    push __cmsg__
    call [printf]
	add esp, 8
    ret