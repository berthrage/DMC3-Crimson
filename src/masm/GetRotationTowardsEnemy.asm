.DATA
extern g_GetRotationTowardsEnemy_ReturnAddr:QWORD
extern g_GetRotationTowardsEnemyCall:QWORD
rotationAdjustedFlag db 0          
previousAction db 0                ; Store the previous action 

.CODE
GetRotationTowardsEnemyDetour PROC
    ; Check the current player action
    mov al, byte ptr [r8 + 3FA4h] ; Load the player's current action 
    cmp al, byte ptr [previousAction] ; Compare it to the previous action
    jne ResetFlag ; If different, reset the flag
    push rax
    push rcx
    push rdx
    push r8
    push r9
    push r10
    push r11

    mov rdx, rcx
    mov rcx, r8
    call qword ptr [g_GetRotationTowardsEnemyCall] ;

    
    ; Check if the rotation was already adjusted
    cmp byte ptr [rotationAdjustedFlag], 1 ; Has the rotation already been adjusted?
    je SkipAdjustment ; If yes, skip adjusting the rotation

    pop r11
    pop r10
    pop r9
    pop r8
    pop rdx
    pop rcx
    pop rax
    cmp word ptr [r8 + 3E84h], 01 ; is player locking on?
    jne Skip
    mov word ptr [r8 + 192],cx
    jmp qword ptr [g_GetRotationTowardsEnemy_ReturnAddr]

ResetFlag:
    ; Update the previous action and reset the flag
    mov byte ptr [previousAction], al ; Store the current action (byte)
    mov byte ptr [rotationAdjustedFlag], 0 ; Reset the flag
    jmp GetRotationTowardsEnemyDetour ; Continue execution

SkipAdjustment:
    ; If the rotation was already adjusted, restore registers and return
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdx
    pop rcx
    pop rax
    jmp qword ptr [g_GetRotationTowardsEnemy_ReturnAddr]

Skip:
    jmp qword ptr [g_GetRotationTowardsEnemy_ReturnAddr]

SetMoveRotationToStick:
    push r9
    mov r9w, word ptr [r8 + 7508h] ; get leftStickPos
    add r9w, word ptr [r8 + 3ED8h] ; add Camera Direction
    sub r9w, 8000h
    cmp word ptr [r8 + 750Ah], 90 ; check leftStick Radius, if neutral then soft lock
    jl  DoNormalSoftLockedAttack
    mov word ptr [r8 + 192], r9w ; set relativeStickRotation
    pop r9
    jmp qword ptr [g_GetRotationTowardsEnemy_ReturnAddr]

DoNormalSoftLockedAttack:
    mov word ptr [r8 + 192],cx
    pop r9
    jmp qword ptr [g_GetRotationTowardsEnemy_ReturnAddr]

GetRotationTowardsEnemyDetour ENDP
END