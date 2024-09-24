.DATA
extern DetourBaseAddr:QWORD
extern createEffectRBXMov:QWORD
extern createEffectCallA:QWORD
extern createEffectCallB:QWORD

.CODE
CreateEffectDetour PROC
	;sub rsp,40h handled by the callee in _fastcall
	; rsp + 56 is the speed argument
	; rsp + 48 is the colo_u_r argument
	; rsp + 40 is the enableCustomColor argument
	mov		QWORD PTR [rsp+32], r9 ; effectBoneIdx
	mov     QWORD PTR [rsp+24], r8 ; effectId
	mov     QWORD PTR [rsp+16], rdx ; effectBank
	mov     QWORD PTR [rsp+8], rcx ; pPlayer
	; rsp+0 is reserved for the return address
	push 	rax
	push	r10
	push	r11
	push 	rdi
	xor 	r8,r8 ; not sure what this is used for
	push 	rbx
	mov 	rbx, [DetourBaseAddr]
	mov 	rbx, [rbx + 0C18AF8h]
	mov 	rbx, [rbx + 0C90E28h]
	test 	rbx, rbx
	je 		return
	mov 	rbx, [rbx + 18h] ; player
	xor 	rcx, rcx
	xor 	rdx, rdx
	mov 	ecx, DWORD PTR [rsp+16+40] ; effectBank
	mov 	edx, DWORD PTR [rsp+24+40] ; effectId

	sub		rsp, 20h ; Shadow space for the call
	call 	createEffectCallA ; from dmc3.exe+211B95, another example at dmc3.exe+20167F
	mov 	rdi, rax
	test 	rax, rax
	je 		return
	cmp		byte ptr [rsp + 40 + 72], 01
	je		EnableCustomColor

ContinueDetour:
	push	r8
	mov		r8d, dword ptr [rax + 0E0h]
	mov		r8d, dword ptr [rsp + 48 + 80] ; adding color
	pop		r8
	sub		rsp, 10h
	movaps	[rsp], xmm3
	movss	xmm3, dword ptr [rsp + 56 + 88] ; adding speed
	movss	dword ptr [rax + 18h], xmm3
	movaps  xmm3, [rsp]
	add		rsp, 10h
	mov 	rcx, rbx
	call 	createEffectCallB
	add		rsp, 20h ; Shadow space for the call
	inc 	eax ; y pos

	push 	r10 ; check player addr was given, temp until all calls are updated
	mov 	r10, QWORD PTR [rsp+8+48] ; pPlayer
	test 	r10, r10
	pop 	r10
	je 		UseP1

	mov 	rbx, QWORD PTR [rsp+8+40] ; pPlayer
	xor 	rax, rax
	cmp     byte ptr [rbx+3E9Bh], 00 ; devilTrigger
	mov     eax, DWORD PTR [rsp+32+40] ; effectBoneIdx ; for skipped bone
	je      skipDTBoneAdd
	mov     eax, 18h
	imul    eax, dword ptr [rbx+3E88h] ; devilTriggerModel
	add 	eax, DWORD PTR [rsp+32+40] ; effectBoneIdx

skipDTBoneAdd:
	mov 	rdx, [rbx + rax * 8h + 0000E5D0h]
	jmp 	Cont

EnableCustomColor:
	mov		word ptr [rax + 0DCh], 2 ;enable customcolor
	jmp		ContinueDetour

UseP1:
	mov 	rdx, [rbx + rax * 8h + 0000E5D0h]
	jmp 	Cont
Cont:
	mov 	rcx, [rdx + 00000110h]
	mov 	[rdi + 000000C0h], rcx ; player xyz
return:
	pop 	rbx
	;add rsp,40h handled by the callee in _fastcall
	pop		rdi
	pop		r11
	pop 	r10
	pop 	rax
	ret
CreateEffectDetour ENDP
END