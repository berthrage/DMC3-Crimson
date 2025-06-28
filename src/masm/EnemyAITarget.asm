INCLUDE CommonMacros.inc
.DATA
extern g_StandardEnemyTarget_ReturnAddr:QWORD
extern g_StandardEnemyTargetCheckCall:QWORD

.CODE
StandardEnemyTargetDetour PROC
    ; EnemySpecificStruct is in RCX
    ; EnemyStruct is in RDI
    ; Player is in RDX
    PushAllXmmExcept xmm0
    PushAllRegs
    mov rcx, [rcx+20h] ; cenemycom to cenemy
    sub rsp, 8
    call qword ptr [g_StandardEnemyTargetCheckCall]  
    add rsp, 8               
    movaps xmm0, [rax+80h]
    jmp Jmpout

Jmpout:
    PopAllRegs
    PopAllXmmExcept xmm0
    jmp qword ptr [g_StandardEnemyTarget_ReturnAddr]

OriginalCode:
    movaps xmm0, [rdx + 80h]

StandardEnemyTargetDetour ENDP


;EnigmaSetTargetAimDetour
.DATA
extern g_EngimaSetAim_ReturnAddr:QWORD
extern g_EnigmaSetAimCheckCall:QWORD

.CODE
EnigmaSetTargetAimDetour PROC
    ; Player in RCX
    ; Pointer to LockedOnEnemyAddr is in R12 + 0x28
    push 	rax
    push    rcx
	push	rdx
	push 	r8
	push	r9
	push	r10
	push	r11
	push	rbx
	push	rdi
    mov rcx, qword ptr [r12+28h]  ;r12 has lockedonenemyaddr - 0x28 as value       
    ; Call the C++ function to decide enemy target
    sub rsp, 28h 
    call qword ptr [g_EnigmaSetAimCheckCall]              
    add rsp, 28h                ; Clean up stack after call
    movaps xmm9, [rax+80h]      ; Move the vec4 data into xmm0
    jmp Jmpout

Jmpout:
    pop		rdi
	pop		rbx
	pop		r11
	pop 	r10
	pop		r9
	pop 	r8
	pop		rdx
    pop     rcx
	pop 	rax
    jmp qword ptr [g_EngimaSetAim_ReturnAddr] ; follow-up should do the trick

EnigmaSetTargetAimDetour ENDP



;EnigmaSetRotationToTargetDetour
.DATA
extern g_EngimaSetRotation_ReturnAddr:QWORD
extern g_EnigmaSetRotationCheckCall:QWORD

.CODE
EnigmaSetRotationToTargetDetour PROC
    ; PlayerPosition in RCX
    ; LockedOnEnemyAddr in RDX
    PushAllXmmExcept xmm0
    PushAllRegsExcept rcx
    add rdx, 60h ; Get the LockedOnEnemyAddr from RBX
    mov rcx, rdx       
    ; Call C++ function
    call qword ptr [g_EnigmaSetRotationCheckCall]
    add rax, 80h 
    mov rcx, rax
    movups  xmm0, [rcx]
    jmp Jmpout
 
Jmpout:
    PopAllRegsExcept rcx
    PopAllXmmExcept xmm0
    subps   xmm0, xmm1
    jmp     qword ptr [g_EngimaSetRotation_ReturnAddr]

OriginalCode:
    movups xmm0, [rcx]
    subps xmm0, xmm1

EnigmaSetRotationToTargetDetour ENDP



;DullahanMaybeUsedDetour
.DATA
extern g_DullahanMaybeUsed_ReturnAddr:QWORD
extern g_DullahanMaybeUsedCheckCall:QWORD

.CODE
DullahanMaybeUsedDetour PROC
    ; PlayerPosition in RCX
    ; LockedOnEnemyAddrPosition in RDX
    PushAllXmmExcept xmm0
    PushAllRegsExcept rcx
    sub rdx, 20h ; Get the EnemyStruct from RDX
    mov rcx, rdx
    sub rsp, 8
    call qword ptr [g_DullahanMaybeUsedCheckCall] 
    add rsp, 8
    add rax, 80h
    mov rcx, rax
    movups xmm0, [rcx]
    jmp Jmpout

Jmpout:
    PopAllRegsExcept rcx
    PopAllXmmExcept xmm0
    mov rsi, rdx
    jmp qword ptr [g_DullahanMaybeUsed_ReturnAddr]

OriginalCode:
    movups xmm0, [rcx]
    mov rsi, rdx
    
DullahanMaybeUsedDetour ENDP


;BeowulfAttackTargetDetour
.DATA
extern g_BeowulfAttackTarget_ReturnAddr:QWORD
extern g_BeowulfAttackTarget_CallAddr:QWORD
extern g_BeowulfAttackTargetCheckCall:QWORD

.CODE
BeowulfAttackTargetDetour PROC
    ; PlayerPosition in RCX 
    ; EnemyPos in RDX
    PushAllRegsExcept rcx
    sub rdx, 20h ; Get the LockedOnEnemyAddr from RDX
    mov rcx, rdx
    sub rsp, 8
    call qword ptr [g_BeowulfAttackTargetCheckCall]  
    add rsp, 8
    add rax, 80h
    mov rcx, rax
    jmp Jmpout

Jmpout:
    PopAllRegsExcept rcx
    call [g_BeowulfAttackTarget_CallAddr]
    jmp qword ptr [g_BeowulfAttackTarget_ReturnAddr]

OriginalCode:
    call [g_BeowulfAttackTarget_CallAddr]
    
BeowulfAttackTargetDetour ENDP


.DATA
extern g_BloodgoyleDiveTargetCheckCall:QWORD
extern g_BloodgoyleDiveTarget_ReturnAddr:QWORD

.CODE
BloodgoyleDiveTargetDetour PROC
    ; EnemyPos is in RCX
    ; EnemyStruct is in RDI+60
    ; PlayerPos is in RDX
    PushAllXmmExcept xmm8
    PushAllRegs
    test rdi, rdi
    je OriginalCode ; If rdi is null, jump to original code
    cmp rdi, 1
    je OriginalCode
    add rdi, 60h ; Get the EnemyStruct from RDI
    mov rcx, rdi
    ;mov rcx, [rcx+20h] ; cenemycom to cenemy
    cmp dword ptr [rcx+18h], 32d ; Check if the enemy is bloodgoyle or soul eater
    jne OriginalCode
    je ApplyTargetPos
    cmp dword ptr [rcx+18h], 34d
    jne OriginalCode
    je ApplyTargetPos
    
ApplyTargetPos:
    call qword ptr [g_BloodgoyleDiveTargetCheckCall]  
    movups xmm8, [rax+80h]
    jmp Jmpout

Jmpout:
    PopAllRegs
    PopAllXmmExcept xmm8
    subps xmm8, xmm0
    jmp qword ptr [g_BloodgoyleDiveTarget_ReturnAddr]

OriginalCode:
    PopAllRegs
    PopAllXmmExcept xmm8
    movups xmm8, [rdx]
    subps xmm8, xmm0
    jmp qword ptr [g_BloodgoyleDiveTarget_ReturnAddr]

BloodgoyleDiveTargetDetour ENDP

; BloodgoyleRotationTargetDetour
.DATA
extern g_BloodgoyleRotationTargetCheckCall:QWORD
extern g_BloodgoyleRotationTarget_ReturnAddr:QWORD
extern g_BloodgoyleRotationTarget_CallAddr:QWORD

.CODE
BloodgoyleRotationTargetDetour PROC
    ; PlayerPosition in RCX 
    ; EnemyPos in RDX
    PushAllXmm
    PushAllRegsExcept rcx
    sub rdx, 20h ; Get the LockedOnEnemyAddr from RDX
    mov rcx, rdx
    sub rsp, 8
    call qword ptr [g_BloodgoyleRotationTargetCheckCall]  
    add rsp, 8
    add rax, 80h
    mov rcx, rax
    jmp Jmpout

Jmpout:
    PopAllRegsExcept rcx
    PopAllXmm
    call [g_BloodgoyleRotationTarget_CallAddr]
    jmp qword ptr [g_BloodgoyleRotationTarget_ReturnAddr]

OriginalCode:
    call [g_BloodgoyleRotationTarget_CallAddr]

BloodgoyleRotationTargetDetour ENDP


; ChessPawnAttackTargetDetour
.DATA
extern g_ChessPawnAttackTargetCheckCall:QWORD
extern g_ChessPawnAttackTarget_ReturnAddr:QWORD
extern g_ChessPawnAttackTarget_CallAddr:QWORD

.CODE
ChessPawnAttackTargetDetour PROC
    ; PlayerPosition in RCX 
    ; EnemyPos in RDX
    PushAllRegsExcept rcx
    sub rdx, 20h ; Get the LockedOnEnemyAddr from RDX
    mov rcx, rdx
    sub rsp, 8
    call qword ptr [g_ChessPawnAttackTargetCheckCall]  
    add rsp, 8
    add rax, 80h
    mov rcx, rax
    jmp Jmpout

Jmpout:
    PopAllRegsExcept rcx
    call [g_ChessPawnAttackTarget_CallAddr]
    jmp qword ptr [g_ChessPawnAttackTarget_ReturnAddr]

OriginalCode:
    call [g_ChessPawnAttackTarget_CallAddr]

ChessPawnAttackTargetDetour ENDP


; ChessKnightAttackTargetDetour
.DATA
extern g_ChessKnightAttackTargetCheckCall:QWORD
extern g_ChessKnightAttackTarget_ReturnAddr:QWORD
extern g_ChessKnightAttackTarget_CallAddr:QWORD

.CODE
ChessKnightAttackTargetDetour PROC
    ; PlayerPosition in RCX 
    ; EnemyPos in RDX
    PushAllRegsExcept rcx
    sub rdx, 20h ; Get the LockedOnEnemyAddr from RDX
    mov rcx, rdx
    sub rsp, 8
    call qword ptr [g_ChessKnightAttackTargetCheckCall]  
    add rsp, 8
    add rax, 80h
    mov rcx, rax
    jmp Jmpout

Jmpout:
    PopAllRegsExcept rcx
    call [g_ChessKnightAttackTarget_CallAddr]
    jmp qword ptr [g_ChessKnightAttackTarget_ReturnAddr]

OriginalCode:
    call [g_ChessKnightAttackTarget_CallAddr]

ChessKnightAttackTargetDetour ENDP



; ChessBishopRotationTargetDetour
.DATA
extern g_ChessBishopRotationTargetCheckCall:QWORD
extern g_ChessBishopRotationTarget_ReturnAddr:QWORD
extern g_ChessBishopRotationTarget_CallAddr:QWORD

.CODE
ChessBishopRotationTargetDetour PROC
    ; PlayerPosition in RCX 
    ; EnemyPos in RDX
    PushAllXmm
    PushAllRegsExcept rcx
    sub rdx, 20h ; Get the LockedOnEnemyAddr from RDX
    mov rcx, rdx
    sub rsp, 8
    call qword ptr [g_ChessBishopRotationTargetCheckCall]  
    add rsp, 8
    add rax, 80h
    mov rcx, rax
    jmp Jmpout

Jmpout:
    PopAllRegsExcept rcx
    PopAllXmm
    call [g_ChessBishopRotationTarget_CallAddr]
    jmp qword ptr [g_ChessBishopRotationTarget_ReturnAddr]

OriginalCode:
    call [g_ChessBishopRotationTarget_CallAddr]

ChessBishopRotationTargetDetour ENDP

; ChessBishopAttackTargetDetour
.DATA
extern g_ChessBishopAttackTargetCheckCall:QWORD
extern g_ChessBishopAttackTarget_ReturnAddr:QWORD
extern g_ChessBishopAttackTarget_CallAddr:QWORD

.CODE
ChessBishopAttackTargetDetour PROC
    ; PlayerPosition in RCX 
    ; EnemyPos in RDX
    PushAllRegsExcept rcx
    sub rdx, 20h ; Get the LockedOnEnemyAddr from RDX
    mov rcx, rdx
    sub rsp, 8
    call qword ptr [g_ChessBishopAttackTargetCheckCall]  
    add rsp, 8
    add rax, 80h
    mov rcx, rax
    jmp Jmpout

Jmpout:
    PopAllRegsExcept rcx
    call [g_ChessBishopAttackTarget_CallAddr]
    jmp qword ptr [g_ChessBishopAttackTarget_ReturnAddr]

OriginalCode:
    call [g_ChessBishopAttackTarget_CallAddr]

ChessBishopAttackTargetDetour ENDP


; ChessBishopAttack2TargetDetour
.DATA
extern g_ChessBishopAttack2TargetCheckCall:QWORD
extern g_ChessBishopAttack2Target_ReturnAddr:QWORD
extern g_ChessBishopAttack2Target_CallAddr:QWORD

.CODE
ChessBishopAttack2TargetDetour PROC
    ; PlayerPosition in R8 
    ; EnemyPos in RDX
    PushAllRegsExcept r8
    sub rdx, 20h ; Get the LockedOnEnemyAddr from RDX
    mov rcx, rdx
    sub rsp, 8
    call qword ptr [g_ChessBishopAttack2TargetCheckCall]  
    add rsp, 8
    add rax, 80h
    mov r8, rax
    jmp Jmpout

Jmpout:
    PopAllRegsExcept r8
    call [g_ChessBishopAttack2Target_CallAddr]
    jmp qword ptr [g_ChessBishopAttack2Target_ReturnAddr]

OriginalCode:
    call [g_ChessBishopAttack2Target_CallAddr]

ChessBishopAttack2TargetDetour ENDP


; ChessBishopAttack2TargetDetour2
.DATA
extern g_ChessBishopAttack2Target2_ReturnAddr:QWORD
extern g_ChessBishopAttack2Target2_CallAddr:QWORD
extern g_ChessBishopAttack2Target2_JmpAddr:QWORD

.CODE
ChessBishopAttack2TargetDetour2 PROC
    ; PlayerPosition in R8 
    mov rdx, r8
    call qword ptr [g_ChessBishopAttack2Target2_JmpAddr]
    jmp Jmpout

Jmpout:
    jmp qword ptr [g_ChessBishopAttack2Target2_ReturnAddr]

OriginalCode:
    call [g_ChessBishopAttack2Target2_CallAddr]

ChessBishopAttack2TargetDetour2 ENDP


; ChessBishopAttack2TargetDetour3
.DATA
extern g_ChessBishopAttack2Target3_ReturnAddr:QWORD
extern g_ChessBishopAttack2Target3_CallAddr:QWORD
extern g_ChessBishopAttack2Target3_JmpAddr:QWORD

.CODE
ChessBishopAttack2TargetDetour3 PROC
    ; PlayerPosition in R8 
    mov rdx, r8
    call qword ptr [g_ChessBishopAttack2Target3_JmpAddr]
    jmp Jmpout

Jmpout:
    jmp qword ptr [g_ChessBishopAttack2Target3_ReturnAddr]

OriginalCode:
    call [g_ChessBishopAttack2Target3_CallAddr]

ChessBishopAttack2TargetDetour3 ENDP


; ChessRookAttackTargetDetour
.DATA
extern g_ChessRookAttackTargetCheckCall:QWORD
extern g_ChessRookAttackTarget_ReturnAddr:QWORD
extern g_ChessRookAttackTarget_CallAddr:QWORD

.CODE
ChessRookAttackTargetDetour PROC
    ; PlayerPosition in R8 
    ; EnemyPos in RDX
    PushAllRegsExcept r8
    sub rdx, 20h ; Get the LockedOnEnemyAddr from RDX
    mov rcx, rdx
    sub rsp, 8
    call qword ptr [g_ChessRookAttackTargetCheckCall]  
    add rsp, 8
    add rax, 80h
    mov r8, rax
    jmp Jmpout

Jmpout:
    PopAllRegsExcept r8
    call [g_ChessRookAttackTarget_CallAddr]
    jmp qword ptr [g_ChessRookAttackTarget_ReturnAddr]

OriginalCode:
    call [g_ChessRookAttackTarget_CallAddr]

ChessRookAttackTargetDetour ENDP


; ChessRookAttackTargetDetour2
.DATA
extern g_ChessRookAttackTarget2_ReturnAddr:QWORD
extern g_ChessRookAttackTarget2_CallAddr:QWORD
extern g_ChessRookAttackTarget2_JmpAddr:QWORD

.CODE
ChessRookAttackTargetDetour2 PROC
    ; PlayerPosition in R8 
    mov rdx, r8
    call qword ptr [g_ChessRookAttackTarget2_JmpAddr]
    jmp Jmpout

Jmpout:
    jmp qword ptr [g_ChessRookAttackTarget2_ReturnAddr]

OriginalCode:
    call [g_ChessRookAttackTarget2_CallAddr]

ChessRookAttackTargetDetour2 ENDP


; ChessRookAttackTargetDetour3
.DATA
extern g_ChessRookAttackTarget3_ReturnAddr:QWORD
extern g_ChessRookAttackTarget3_CallAddr:QWORD
extern g_ChessRookAttackTarget3_JmpAddr:QWORD

.CODE
ChessRookAttackTargetDetour3 PROC
    ; PlayerPosition in R8 
    mov rdx, r8
    call qword ptr [g_ChessRookAttackTarget3_JmpAddr]
    jmp Jmpout

Jmpout:
    jmp qword ptr [g_ChessRookAttackTarget3_ReturnAddr]

OriginalCode:
    call [g_ChessRookAttackTarget3_CallAddr]

ChessRookAttackTargetDetour3 ENDP


; ChessKingAttackTargetDetour
.DATA
extern g_ChessKingAttackTargetCheckCall:QWORD
extern g_ChessKingAttackTarget_JmpAddr:QWORD
extern g_ChessKingAttackTarget_ReturnAddr:QWORD
extern g_ChessKingAttackTarget_CallAddr:QWORD

.CODE
ChessKingAttackTargetDetour PROC
    ; EnemyPos in RCX
    ; Gotta load PlayerPos into RDX
    PushAllRegsExcept rdx
    sub rcx, 20h ; Get the LockedOnEnemyAddr from RCX
    sub rsp, 8
    call qword ptr [g_ChessKingAttackTargetCheckCall]  
    add rsp, 8
    add rax, 80h
    mov rdx, rax
    jmp Jmpout

Jmpout:
    PopAllRegsExcept rdx
    call [g_ChessKingAttackTarget_JmpAddr]
    jmp qword ptr [g_ChessKingAttackTarget_ReturnAddr]

OriginalCode:
    call [g_ChessKingAttackTarget_CallAddr]

ChessKingAttackTargetDetour ENDP


;SoulEaterGrabTargetDetour
.DATA
extern g_SoulEaterGrabTargetCheckCall:QWORD
extern g_SoulEaterGrabTarget_ReturnAddr:QWORD

.CODE
SoulEaterGrabTargetDetour PROC
    ; EnemyPos in RBX
    PushAllXmmExcept xmm0
    PushAllRegs
    sub rbx, 20h ; Get the EnemyStruct from RBX
    mov rcx, rbx
    call qword ptr [g_SoulEaterGrabTargetCheckCall]  
    movaps xmm0, [rax+80h]
    jmp Jmpout

Jmpout:
    PopAllRegs
    PopAllXmmExcept xmm0
    jmp qword ptr [g_SoulEaterGrabTarget_ReturnAddr]

OriginalCode:
    movaps xmm0,[rcx+80h]

SoulEaterGrabTargetDetour ENDP


; EnigmaActualRotationTargetDetour
.DATA
extern g_EnigmaActualRotationTargetCheckCall:QWORD
extern g_EnigmaActualRotationTarget_ReturnAddr:QWORD
extern g_EnigmaActualRotationTarget_CallAddr:QWORD

.CODE
EnigmaActualRotationTargetDetour PROC
    ; PlayerPosition in RCX 
    ; EnemyPos in RDX
    PushAllRegsExcept rcx
    sub rdx, 20h ; Get the LockedOnEnemyAddr from RDX
    mov rcx, rdx
    sub rsp, 8
    call qword ptr [g_EnigmaActualRotationTargetCheckCall]  
    add rsp, 8
    add rax, 80h
    mov rcx, rax
    jmp Jmpout

Jmpout:
    PopAllRegsExcept rcx
    call [g_EnigmaActualRotationTarget_CallAddr]
    jmp qword ptr [g_EnigmaActualRotationTarget_ReturnAddr]

OriginalCode:
    call [g_EnigmaActualRotationTarget_CallAddr]

EnigmaActualRotationTargetDetour ENDP


;EnigmaActualAimTargetDetour
.DATA
extern g_EnigmaActualAimTargetCheckCall:QWORD
extern g_EnigmaActualAimTarget_ReturnAddr:QWORD

.CODE
EnigmaActualAimTargetDetour PROC
    ; EnemyAddr in RCX+30h
    PushAllRegsExcept rax
    add rax, 60h ; Get the LockOnEnemyAddr from RCX
    mov rcx, rax
    sub rsp, 8
    call qword ptr [g_EnigmaActualAimTargetCheckCall]  
    add rsp, 8
    movaps xmm0,[rax+80h]
    jmp Jmpout

Jmpout:
    PopAllRegsExcept rax
    jmp qword ptr [g_EnigmaActualAimTarget_ReturnAddr]

OriginalCode:
    movaps xmm0,[rax+80h]

EnigmaActualAimTargetDetour ENDP


;ArachneCirclingAroundDetour
.DATA
extern g_ArachneCirclingAroundCheckCall:QWORD
extern g_ArachneCirclingAround_ReturnAddr:QWORD

.CODE
ArachneCirclingAroundDetour PROC
    ; Player in RBX
    ; LockedOnEnemyAddr in RDI+60h
    PushAllXmmExcept xmm0
    PushAllRegs
    add rdi, 60h ; Get the EnemyStruct from RDI
    mov rcx, rdi
    call qword ptr [g_ArachneCirclingAroundCheckCall]  
    movaps xmm0, [rax+80h]
    jmp Jmpout

Jmpout:
    PopAllRegs
    PopAllXmmExcept xmm0
    jmp qword ptr [g_ArachneCirclingAround_ReturnAddr]

OriginalCode:
    movaps xmm0, [rbx+80h]

ArachneCirclingAroundDetour ENDP


;ArachneJumpAttackDetour
.DATA
extern g_ArachneJumpAttackCheckCall:QWORD
extern g_ArachneJumpAttack_ReturnAddr:QWORD

.CODE
ArachneJumpAttackDetour PROC
    ; Player in RDX
    ; LockedOnEnemyAddr in RCX+60h
    PushAllXmmExcept xmm0
    PushAllRegs
    add rcx, 60h ; Get the LockedOnEnemyAddr from RCX
    call qword ptr [g_ArachneJumpAttackCheckCall]  
    movaps xmm0, [rax+80h]
    jmp Jmpout

Jmpout:
    PopAllRegs
    PopAllXmmExcept xmm0
    jmp qword ptr [g_ArachneJumpAttack_ReturnAddr]

OriginalCode:
    movaps xmm0, [rdx+80h]

ArachneJumpAttackDetour ENDP


;VergilBlinkPositionDetour
.DATA
extern g_VergilBlinkPositionCheckCall:QWORD
extern g_VergilBlinkPosition_ReturnAddr:QWORD
float_temp dq 0.0

.CODE
VergilBlinkPositionDetour PROC
    ; Player in RCX
    ; LockedOnEnemyAddr in RDI+60h
    PushAllXmm
    PushAllRegs
    add rdi, 60h ; Get the LockedOnEnemyAddr from RDI
    mov rcx, rdi
    call qword ptr [g_VergilBlinkPositionCheckCall]  
    movaps xmm1, dword ptr [rax+80h] 
    movaps dword ptr [float_temp], xmm1 ; preserve rax+80h state
    jmp Jmpout

Jmpout:
    PopAllRegs
    PopAllXmm
    addps xmm0, dword ptr [float_temp]
    jmp qword ptr [g_VergilBlinkPosition_ReturnAddr]

OriginalCode:
    addps xmm0, [rcx+80h]

VergilBlinkPositionDetour ENDP


;FixMPLockOnDetour
.DATA
extern g_FixMPLockOn_ReturnAddr:QWORD
extern g_FixMPLockOnCheckCall:QWORD

.CODE
FixMPLockOnDetour PROC
    ; Player in RBX (+6328h is lockOnTarget stores enemy addr)
    ; LockedOnEnemyAddr in RCX
    push    rax
    push    rdx
    push    r8
    push    r9
    push    r10
    push    r11
    push    rbx
    push    rdi
    push    r15

    sub     rsp, 48h          ; Reserve space for xmm1–xmm3
    movdqu  [rsp+00h], xmm1
    movdqu  [rsp+10h], xmm2
    movdqu  [rsp+20h], xmm3

    mov r15, rbx
    mov rcx, rbx
    call    qword ptr [g_FixMPLockOnCheckCall]

    ; Use result in rax
    mov     rcx, rax
    mov     [r15+6328h], rcx ; Store the enemy address in the lockOnTarget]

    ; Restore xmm registers
    movdqu  xmm1, [rsp+00h]
    movdqu  xmm2, [rsp+10h]
    movdqu  xmm3, [rsp+20h]
    add     rsp, 48h

    ; Restore other registers
    pop     r15
    pop     rdi
    pop     rbx
    pop     r11
    pop     r10
    pop     r9
    pop     r8
    pop     rdx
    pop     rax 
    jmp     qword ptr [g_FixMPLockOn_ReturnAddr]
FixMPLockOnDetour ENDP

END