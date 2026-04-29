INCLUDE CommonMacros.inc
.DATA
EXTERN g_ShotgunShlSpawnAnglePointBlank_ReturnAddr1:QWORD
EXTERN g_ShotgunShlSpawnAnglePointBlank_ReturnAddr2:QWORD
EXTERN g_ShotgunShlSpawnAnglePointBlankCheckCall:QWORD
EXTERN g_ShotgunShlSpawnAnglePointBlankLockedOnEnemyJmp:QWORD
EXTERN g_PointBlankShotgunFire_ReturnAddr:QWORD
EXTERN g_PointBlankShotgunFireTailCall_ReturnAddr:QWORD
EXTERN g_PointBlankShotgunFireDelayCall:QWORD
EXTERN g_PointBlankShotgunFireOgCall:QWORD
EXTERN g_PointBlankShotgunCancelAnimTailCall:QWORD
originalJumpAddr dq 0
newJumpAddr dq 0

.CODE
ShotgunShlSpawnAnglePointBlankDetour PROC
	PushAllRegs
	sub		rsp, 20h ; Shadow space for the call
	mov rcx, rbx ; playerAddr in rbx
	call qword ptr [g_ShotgunShlSpawnAnglePointBlankCheckCall] ; Check if in Backslide
	add rsp, 20h
	cmp al, 1
	je AlterAngle
	PopAllRegs
	jmp OriginalCode
	
AlterAngle:
	PopAllRegs
	movzx ecx, word ptr[rbx + 0C0h]
	add cx, 8000h ; Add 180 degrees to the angle
	jmp [g_ShotgunShlSpawnAnglePointBlank_ReturnAddr1]

OriginalCode:
	jne CheckLockOnEnemy
	movzx ecx, word ptr[rbx + 0C0h]
	jmp [g_ShotgunShlSpawnAnglePointBlank_ReturnAddr1]

CheckLockOnEnemy:
	jmp [g_ShotgunShlSpawnAnglePointBlankLockedOnEnemyJmp]

ShotgunShlSpawnAnglePointBlankDetour ENDP


.CODE
ShotgunShlSpawnAnglePointBlankDetour2 PROC
	PushAllRegs
	sub		rsp, 20h ; Shadow space for the call
	mov rcx, rbx ; playerAddr in rbx
	call qword ptr [g_ShotgunShlSpawnAnglePointBlankCheckCall] ; Check if in Backslide
	add rsp, 20h
	cmp al, 1
	je AlterAngle
	PopAllRegs
	jmp OriginalCode
	
AlterAngle:
	PopAllRegs
	movzx ecx, word ptr[rbx + 0C0h]
	add cx, 8000h ; Add 180 degrees to the angle
	jmp [g_ShotgunShlSpawnAnglePointBlank_ReturnAddr2]

OriginalCode:
	movzx ecx, word ptr[rbx + 0C0h]
	jmp [g_ShotgunShlSpawnAnglePointBlank_ReturnAddr2]


ShotgunShlSpawnAnglePointBlankDetour2 ENDP


.CODE
PointBlankShotgunFireDetour PROC
	PushAllRegs
	sub		rsp, 20h ; Shadow space for the call
	mov rcx, rbx ; playerAddr in rbx
	call qword ptr [g_ShotgunShlSpawnAnglePointBlankCheckCall] ; Check if in Backslide
	add rsp, 20h
	cmp al, 1
	je DelayCall
	PopAllRegs
	jmp OriginalCode
	
DelayCall:
	PopAllRegs
	PushAllRegs
	sub		rsp, 20h ; Shadow space for the call
	mov rcx, rbx
	call qword ptr [g_PointBlankShotgunFireDelayCall] ; Queue the delay function to prevent the shotgun from firing on his foot
	add rsp, 20h
	PopAllRegs
	jmp NewJump

OriginalCode:
	jne OriginalJump
	lea rcx, [rbx+6510h] 
	jmp [g_PointBlankShotgunFire_ReturnAddr]

OriginalJump:
	push r13
	mov r13, 14020EE09h
	mov qword ptr [originalJumpAddr], r13
	pop r13
	jmp originalJumpAddr

NewJump:
	push r13
	mov r13, 14020EE16h
	mov qword ptr [newJumpAddr], r13
	pop r13
	jmp newJumpAddr

PointBlankShotgunFireDetour ENDP


.CODE
PointBlankShotgunFireTailCallDetour PROC
	PushAllRegs
	sub		rsp, 20h ; Shadow space for the call
	call qword ptr [g_ShotgunShlSpawnAnglePointBlankCheckCall] ; Check if in Backslide
	add rsp, 20h
	cmp al, 1
	je DelayCall
	PopAllRegs
	jmp OriginalCode
	
DelayCall:
	PopAllRegs
	PushAllRegs
	sub		rsp, 20h ; Shadow space for the call
	call qword ptr [g_PointBlankShotgunFireDelayCall] ; Queue the delay function to prevent the shotgun from firing on his foot
	add rsp, 20h
	PopAllRegs
	jmp [g_PointBlankShotgunCancelAnimTailCall] ; Jump to the tail call that cancels the shotgun fire animation to prevent it from playing on his foot

OriginalCode:
	jmp [g_PointBlankShotgunFireOgCall]
PointBlankShotgunFireTailCallDetour ENDP

END