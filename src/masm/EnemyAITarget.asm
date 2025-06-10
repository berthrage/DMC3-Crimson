.DATA
extern g_StandardEnemyTarget_ReturnAddr:QWORD
extern g_StandardEnemyTargetCheckCall:QWORD

.CODE
StandardEnemyTargetDetour PROC
    ; EnemySpecificStruct is in RCX
    ; EnemyStruct is in RDI
    ; Player is in RDX
    push 	rax
    push    rcx
	push	rdx
	push 	r8
	push	r9
	push	r10
	push	r11
	push	rbx
	push	rdi
    ;sub     rsp, 20h             ; Allocate space for xmm1 (16 bytes), align stack
    ;movdqu  [rsp], xmm1          ; Save xmm1
    call qword ptr [g_StandardEnemyTargetCheckCall]  
    movaps xmm0, [rax+80h]
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



;BeowulfSetTargetAttackDetour
.DATA
extern g_BeowulfSetTarget_ReturnAddr:QWORD
extern g_BeowulfSetTargetCheckCall:QWORD

.CODE
BeowulfSetTargetAttackDetour PROC
    ; PlayerPosition in RCX
    ; LockedOnEnemyAddrPosition in RDX
    push    rax
    push    rcx
    push    rdx
    push    r8
    push    r9
    push    r10
    push    r11
    push    rbx
    push    rdi

    sub     rsp, 48h          ; Reserve space for xmm1–xmm3
    movdqu  [rsp+00h], xmm1
    movdqu  [rsp+10h], xmm2
    movdqu  [rsp+20h], xmm3

    mov     r13, rdx
    mov     r14, [r13-80h]
    mov     rcx, r14
    call    qword ptr [g_BeowulfSetTargetCheckCall]

    ; Use result in rax
    mov     rcx, rax
    movups  xmm0, [rcx+80h]

    ; Restore xmm registers
    movdqu  xmm1, [rsp+00h]
    movdqu  xmm2, [rsp+10h]
    movdqu  xmm3, [rsp+20h]
    add     rsp, 48h

    ; Restore other registers
    pop     rdi
    pop     rbx
    pop     r11
    pop     r10
    pop     r9
    pop     r8
    pop     rdx
    pop     rcx
    pop     rax
    mov     rsi, rdx         
    jmp     qword ptr [g_BeowulfSetTarget_ReturnAddr]
BeowulfSetTargetAttackDetour ENDP



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