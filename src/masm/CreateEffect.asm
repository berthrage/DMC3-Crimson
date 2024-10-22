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
	push 	rcx
	push	rdx
	push 	r8
	push	r9
	push	r10
	push	r11
	push	rbx
	push	rdi
	
	xor 	r8,r8 ; not sure what this is used for
	mov 	rbx, [DetourBaseAddr]
	mov 	rbx, [rbx + 0C18AF8h]
	mov 	rbx, [rbx + 0C90E28h]
	test 	rbx, rbx
	je 		return
	mov 	rbx, [rbx + 18h] ; player
	xor 	rcx, rcx
	xor 	rdx, rdx
	mov 	ecx, DWORD PTR [rsp+16+72] ; effectBank
	mov 	edx, DWORD PTR [rsp+24+72] ; effectId

	sub		rsp, 20h ; Shadow space for the call
	call 	createEffectCallA ; from dmc3.exe+211B95, another example at dmc3.exe+20167F
	mov 	rdi, rax
	test 	rax, rax
	je 		ReturnWStackRestore
	cmp		byte ptr [rsp + 40 + 104], 01
	je		EnableCustomColor

ContinueDetour:
	push	r8
	mov		r8d, dword ptr [rsp + 48 + 112] ; adding color
	mov		dword ptr [rax + 0E0h], r8d
	pop		r8
	sub		rsp, 10h
	movaps	[rsp], xmm3
	movss	xmm3, dword ptr [rsp + 56 + 120] ; adding speed
	movss	dword ptr [rax + 18h], xmm3
	movaps  xmm3, [rsp]
	add		rsp, 10h
	mov 	rcx, rbx
	call 	createEffectCallB
	add		rsp, 20h ; Shadow space for the call
	inc 	eax ; y pos

	push 	r10 ; check player addr was given, temp until all calls are updated
	mov 	r10, QWORD PTR [rsp+8+80] ; pPlayer
	test 	r10, r10
	pop 	r10
	je 		UseP1

	mov 	rbx, QWORD PTR [rsp+8+72] ; pPlayer
	xor 	rax, rax
	cmp     byte ptr [rbx+3E9Bh], 00 ; if not in devilTrigger
	je		IsDoppelgangerCheck
	jmp		DoDevilTriggerBoneAdd
	
DoDevilTriggerBoneAdd:
	mov     eax, 18h
	imul    eax, dword ptr [rbx+3E88h] ; devilTriggerModel
	add 	eax, DWORD PTR [rsp+32+72] ; effectBoneIdx
	jmp		AddBone

IsDoppelgangerCheck:
	cmp		byte ptr [rbx + 1CACEh], 00 ; checking if not a Clone since Clone uses DT Model
	mov     eax, DWORD PTR [rsp+32+72] ; effectBoneIdx ; for skipped bone
	je      AddBone
	jmp		DoDevilTriggerBoneAdd

AddBone:
	mov 	rdx, [rbx + rax * 8h + 0000E5D0h]
	jmp 	EndFunc

EnableCustomColor:
	mov		word ptr [rax + 0DCh], 2 ;enable customcolor
	jmp		ContinueDetour

UseP1:
	mov 	rdx, [rbx + rax * 8h + 0000E5D0h]
	jmp 	EndFunc

ReturnWStackRestore:
	add		rsp, 20h
	pop		rdi
	pop		rbx
	pop		r11
	pop 	r10
	pop		r9
	pop 	r8
	pop		rdx
	pop		rcx
	pop 	rax
	ret

EndFunc:
	mov 	rcx, [rdx + 00000110h]
	mov 	[rdi + 000000C0h], rcx ; player xyz
	; goes to return

return:
	pop		rdi
	pop		rbx
	pop		r11
	pop 	r10
	pop		r9
	pop 	r8
	pop		rdx
	pop		rcx
	pop 	rax
	ret
	;add rsp,40h handled by the callee in _fastcall
CreateEffectDetour ENDP
END