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
    ; LockedOnEnemyAddr in RBX
    push    rax
    push    rdx
    push    r8
    push    r9
    push    r10
    push    r11
    push    rbx
    push    rdi

    sub     rsp, 20h             ; Allocate space for xmm1 (16 bytes), align stack
    movdqu  [rsp], xmm1          ; Save xmm1

    mov     rcx, rbx
    sub     rsp, 28h             ; shadow space (20h) + 8 for alignment
    ; Call C++ function
    call    qword ptr [g_EnigmaSetRotationCheckCall]
    add     rsp, 28h

    ; Use result in rax
    mov     rcx, rax
    movups  xmm0, [rcx+80h]

    movdqu  xmm1, [rsp]          ; Restore xmm1
    add     rsp, 20h             ; Restore stack from xmm1 save

    subps   xmm0, xmm1

    ; Restore registers
Jmpout:
    pop     rdi
    pop     rbx
    pop     r11
    pop     r10
    pop     r9
    pop     r8
    pop     rdx
    pop     rax
    jmp     qword ptr [g_EngimaSetRotation_ReturnAddr]

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
    PushAllRegs
    sub rdx, 20h ; Get the EnemyStruct from RDX
    mov rcx, rdx
    call qword ptr [g_DullahanMaybeUsedCheckCall]  
    movups xmm0, [rax+80h]
    jmp Jmpout

Jmpout:
    PopAllRegs
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
extern g_BeowulfAttackTargetCheckCall:QWORD

.CODE
BeowulfAttackTargetDetour PROC
    ; PlayerPosition in RCX and RDX
    ; PlayerStruct in RBX
    ; we gotta take LockedOnEnemyAddr from PlayerStruct
    ;PushAllXmmExcept xmm0
    PushAllRegs
    add rbx, 6328h ; Get the EnemyStruct from RDX
    mov rcx, [rbx]
    sub rsp, 8 
    call qword ptr [g_BeowulfAttackTargetCheckCall]  
    add rsp, 8
    movss xmm1, dword ptr [rax+80h]
    movss xmm0, dword ptr [rax+88h]
    subss xmm0, dword ptr [rax+130h] 
    subss xmm1, dword ptr [rax+138h]

    jmp Jmpout

Jmpout:
    PopAllRegs
    ;PopAllXmmExcept xmm0
    jmp qword ptr [g_BeowulfAttackTarget_ReturnAddr]

OriginalCode:
    movss xmm1, dword ptr [rcx]
    movss xmm0, dword ptr [rcx+08h]
    subss xmm0, dword ptr[rdx+08h]
    subss xmm1, dword ptr [rdx]
    
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
    add rdi, 60h ; Get the EnemyStruct from RDI
    ;add rdi, 60h
    mov rcx, rdi
    ;mov rcx, [rcx+20h] ; cenemycom to cenemy
    call qword ptr [g_BloodgoyleDiveTargetCheckCall]  
    movups xmm8, [rax+80h]
    jmp Jmpout

Jmpout:
    PopAllRegs
    PopAllXmmExcept xmm8
    subps xmm8, xmm0
    jmp qword ptr [g_BloodgoyleDiveTarget_ReturnAddr]

OriginalCode:
    movups xmm8, [rdx]
    subps xmm8, xmm0

BloodgoyleDiveTargetDetour ENDP


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