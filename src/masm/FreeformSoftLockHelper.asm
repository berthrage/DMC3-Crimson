.DATA
extern g_FreeformSoftLockHelper_ReturnAddr:QWORD
rotationAdjustedFlag db 0          
previousAction db 0                ; Store the previous action 

.CODE
FreeformSoftLockHelperDetour PROC
    ; Check the current player action
    mov al, byte ptr [r8 + 3FA4h] ; Load the player's current action 
    cmp al, byte ptr [previousAction] ; Compare it to the previous action
    jne ResetFlag ; If different, reset the flag

    
    ; Check if the rotation was already adjusted
    cmp byte ptr [rotationAdjustedFlag], 1 ; Has the rotation already been adjusted?
    je Skip ; If yes, skip adjusting the rotation


    cmp word ptr [r8 + 3E84h], 01 ; is player locking on?
    jmp Skip
    mov word ptr [r8 + 192],cx
    jmp qword ptr [g_FreeformSoftLockHelper_ReturnAddr]

ResetFlag:
    ; Update the previous action and reset the flag
    mov byte ptr [previousAction], al ; Store the current action (byte)
    mov byte ptr [rotationAdjustedFlag], 0 ; Reset the flag
    jmp FreeformSoftLockHelperDetour ; Continue execution

Skip:
    jmp qword ptr [g_FreeformSoftLockHelper_ReturnAddr]

SetMoveRotationToStick:
    ; Unused, keeping it here for preveservation if needed.
    push r9
    mov r9w, word ptr [r8 + 7508h] ; get leftStickPos
    add r9w, word ptr [r8 + 3ED8h] ; add Camera Direction
    sub r9w, 8000h
    cmp word ptr [r8 + 750Ah], 90 ; check leftStick Radius, if neutral then soft lock
    jl  DoNormalSoftLockedAttack
    mov word ptr [r8 + 192], r9w ; set relativeStickRotation
    pop r9
    jmp qword ptr [g_FreeformSoftLockHelper_ReturnAddr]

DoNormalSoftLockedAttack:
    mov word ptr [r8 + 192],cx
    pop r9
    jmp qword ptr [g_FreeformSoftLockHelper_ReturnAddr]

FreeformSoftLockHelperDetour ENDP
END