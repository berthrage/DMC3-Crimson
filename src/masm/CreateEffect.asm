.CODE
extern DetourBaseAddr:QWORD
extern createEffectRBXMov:QWORD
extern createEffectCallA:QWORD
extern createEffectCallB:QWORD
extern createEffectBank:DWORD
extern createEffectID:DWORD
extern createEffectBone:DWORD
extern createEffectPlayerAddr:QWORD

CreateEffectDetour PROC
	sub rsp,40h
	xor r8,r8 ; not sure what this is used for
	push rbx
	mov rbx, [DetourBaseAddr]
	mov rbx, [rbx + 0C18AF8h]
	mov rbx, [rbx + 0C90E28h]
	test rbx,rbx
	je return
	mov rbx, [rbx + 18h] ; player
	xor rcx, rcx
	xor rdx, rdx
	mov ecx, [createEffectBank]
	mov edx, [createEffectID]
	; where add colour
	call createEffectCallA ; from dmc3.exe+211B95, another example at dmc3.exe+20167F
	mov rdi, rax
	test rax, rax
	je return
	mov rcx, rbx
	call createEffectCallB
	inc eax ; y pos

	push r10 ; check player addr was given, temp until all calls are updated
	mov r10, [createEffectPlayerAddr]
	test r10, r10
	pop r10
	je UseP1

	mov rdx, [createEffectPlayerAddr] ; manual player addr
	xor rax, rax
	mov eax, createEffectBone

	mov rdx, [rbx + rax * 8h + 0000E5D0h]
	jmp Cont

UseP1:
	mov rdx, [rbx + rax * 8h + 0000E5D0h]
	jmp Cont
Cont:
	mov [createEffectPlayerAddr], 0
	mov rcx, [rdx + 00000110h]
	mov [rdi + 000000C0h],rcx ; player xyz
return:
	pop rbx
	add rsp,40h
	ret
CreateEffectDetour ENDP
END