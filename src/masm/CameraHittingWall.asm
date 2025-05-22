.DATA
extern g_CamHittingWall_ReturnAddr:QWORD
extern g_CamHittingWall_ConditionalAddr:QWORD

.CODE
CamHittingWallDetour PROC 
    push rcx
    mov r14d,r15d
    mov rcx, g_CamHittingWall_ConditionalAddr          ; Load address of pointer
    test eax,eax
    mov byte ptr [rcx], al                             ; Store result in *g_cameraHittingWall
    
    
JmpOut:
    pop rcx
    jmp qword ptr [g_CamHittingWall_ReturnAddr]

CamHittingWallDetour ENDP
END
