.DATA
extern g_MPCameraPos_ReturnAddr:QWORD
extern g_MPCameraPos_NewPosAddr:QWORD
extern g_MPCamCheckCall:QWORD

.CODE
MultiplayerCameraPositioningDetour PROC
    push rax
    push rcx
    push rdx
    push rdi
    push r8
    push r9
    sub rsp, 20h              ; Allocate space for xmm0 and xmm1
    movaps [rsp+10h], xmm1    ; Store xmm1
    movaps [rsp], xmm0        ; Store xmm0

    ; Call the C++ function to Check if 1P's distant enough from other players
    call qword ptr [g_MPCamCheckCall]
    
    ; Ensure cmp al, 01 remains intact and works as expected
    cmp al, 01
    jne OriginalCamera        ; Jump to OriginalCamera if cmp fails

    ; If equal, continue with MultiplayerCamera logic
    movaps xmm1, [rsp+10h]    ; Restore xmm1
    movaps xmm0, [rsp]        ; Restore xmm0
    add rsp, 20h              ; Clean up stack space
    pop r9
    pop r8
    pop rdi
    pop rdx
    pop rcx
    pop rax
    jmp MultiplayerCamera

OriginalCamera:
    ; Restore camera to 1P's position
    movaps xmm1, [rsp+10h]    ; Restore xmm1
    movaps xmm0, [rsp]        ; Restore xmm0
    add rsp, 20h              ; Clean up stack space
    pop r9
    pop r8
    pop rdi
    pop rdx
    pop rcx
    pop rax
    movaps xmm0, [rdx + 80h]
    jmp qword ptr [g_MPCameraPos_ReturnAddr]

MultiplayerCamera:
    push r9
    push rcx
    mov r9, g_MPCameraPos_NewPosAddr
    movaps xmm0, [r9]
    pop rcx
    pop r9
    jmp qword ptr [g_MPCameraPos_ReturnAddr]

MultiplayerCameraPositioningDetour ENDP
END