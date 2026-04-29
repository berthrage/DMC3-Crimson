INCLUDE CommonMacros.inc
.DATA
extern g_DTMustStyleArmor_ReturnAddr:QWORD
extern g_DTMustStyleArmor_CheckCall1:QWORD
extern g_DTMustStyleArmor_CheckCall2:QWORD

.CODE
DTMustStyleArmorDetour PROC
	PushAllRegs
	call qword ptr [g_DTMustStyleArmor_CheckCall1] ; check if in must style mode
	cmp al, 0
	jne CheckIfInDT
	PopAllRegs
	jmp OriginalCode


CheckIfInDT:
	PopAllRegs
	cmp byte ptr [rsi+3E9Bh], 1 ; hit player's dt
	jne OriginalCode
	ret

OriginalCode:
	mov edx,[r8+rax*4+0027A1D4h] ; before switch for each style rank loss
	PushAllRegs
	call qword ptr [g_DTMustStyleArmor_CheckCall2] ; set announcer was hit for all ranks
	PopAllRegs
	jmp [g_DTMustStyleArmor_ReturnAddr]


DTMustStyleArmorDetour ENDP

END