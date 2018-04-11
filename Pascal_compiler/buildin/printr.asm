printr:
	push dword [esp+4]
    push __fmsg__
    call [printf]
	add esp, 4
    ret