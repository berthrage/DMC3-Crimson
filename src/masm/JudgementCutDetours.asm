INCLUDE CommonMacros.inc
.DATA
EXTERN g_JudgementCutSpeed_ReturnAddr:QWORD
EXTERN g_JudgementCutStartDelayCall:QWORD
EXTERN g_JudgementCutSpawnCollisionCall:QWORD
EXTERN g_JudgementCutCheckJustFrameCall:QWORD
EXTERN g_JudgementCutVFX_ReturnAddr:QWORD
EXTERN g_JudgementCutVFX_ReturnAddr2:QWORD
EXTERN g_JudgementCutRegularVFXCall:QWORD
EXTERN g_JudgementCutJustFrameVFXCall:QWORD
EXTERN g_JudgementCutPosition_ReturnAddr:QWORD
EXTERN g_JudgementCutSetPositionCall:QWORD
EXTERN g_JudgementCutExtraShl_ReturnAddr:QWORD
EXTERN g_JudgementCutExtraShlCall:QWORD
cgeneratorptr dq 0

.CODE
JudgementCutSpeedDetour PROC
	PushAllRegs
	sub		rsp, 20h ; Shadow space for the call
	mov rcx, qword ptr [rcx+530h] ; playerAddr in rcx (shlActorAddr) + 530h
	call qword ptr [g_JudgementCutCheckJustFrameCall] ; Check if in JustFrameJDC
	add rsp, 20h
	cmp al, 1
	je ChangeDelay
	PopAllRegs
	jmp OriginalCode
	
ChangeDelay:
	PopAllRegs
	xor r11, r11
	mov r11, 42a00000h ; 80.0f in hex
	mov qword ptr [rcx+528h], r11 ; Set jdcDelay to 80.0f
	jmp OriginalCode

OriginalCode:
	jmp [g_JudgementCutStartDelayCall]

JudgementCutSpeedDetour ENDP


.CODE
JudgementCutVFXDetour PROC
	PushAllRegs
	sub		rsp, 20h ; Shadow space for the call
	mov rcx, qword ptr [rbx+4D0h] ; playerAddr in rbx (shlAddr) + 530h
	call qword ptr [g_JudgementCutCheckJustFrameCall] ; Check if in JustFrameJDC
	add rsp, 20h
	cmp al, 1
	je PlayJustFrameJDCVFX
	PopAllRegs
	jmp OriginalCode
	
PlayJustFrameJDCVFX:
	PopAllRegs
	PushAllRegs
	mov rcx, rbx
	call [g_JudgementCutJustFrameVFXCall]
	mov qword ptr [cgeneratorptr], rax
	PopAllRegs
	mov rax, qword ptr [cgeneratorptr]
	jmp [g_JudgementCutVFX_ReturnAddr2]

OriginalCode:
	call [g_JudgementCutRegularVFXCall]
	jmp [g_JudgementCutVFX_ReturnAddr]

JudgementCutVFXDetour ENDP


.CODE
JudgementCutPositionDetour PROC
	PushAllRegs
	sub		rsp, 20h ; Shadow space for the call
	mov rcx, rbx ; shlAddr
	call qword ptr [g_JudgementCutSetPositionCall] ; New Tracking
	add rsp, 20h
	PopAllRegs
	jmp OriginalCode

OriginalCode:
	mov qword ptr [rbx+17Ch], 3F800000h ; Set W position to 1.0f
	jmp [g_JudgementCutPosition_ReturnAddr]

JudgementCutPositionDetour ENDP


.CODE
JudgementCutExtraShlDetour PROC
	PushAllRegs
	sub		rsp, 20h ; Shadow space for the call
	mov rcx, qword ptr [rbx+530h] ; playerAddr in rbx (shlActorAddr) + 530h
	call qword ptr [g_JudgementCutCheckJustFrameCall] ; Check if in JustFrameJDC
	add rsp, 20h
	cmp al, 1
	je AddExtraJDCs
	PopAllRegs
	jmp OriginalCode
	
AddExtraJDCs:
	PopAllRegs
	PushAllRegs
	mov rcx, rbx ; shlAddr in rbx
	sub        rsp, 20h ; Shadow space for the call
	call qword ptr [g_JudgementCutExtraShlCall] ; Add extra collisions for JustFrameJDC
	add        rsp, 20h
	PopAllRegs
	jmp OriginalCode

OriginalCode:
	 movzx edx, byte ptr [rbx+08]
	 test edx, edx
	 jmp [g_JudgementCutExtraShl_ReturnAddr]

JudgementCutExtraShlDetour ENDP

END