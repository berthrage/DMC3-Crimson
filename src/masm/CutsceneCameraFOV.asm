.DATA
extern g_CutsceneCameraFOV_ReturnAddr:QWORD
extern g_CutsceneCameraFOV_MultiplierAddr:QWORD
extern g_CutsceneCameraFOV_GameplayReturnAddr:QWORD
extern g_CutsceneCameraFOV_PreviewReturnAddr:QWORD

.CODE
CutsceneCameraFOVDetour PROC

    ; SetSceneCameraFOV takes the FOV of whatever camera is being applied. Only the
    ; gameplay camera update and the menu model preview bring their own FOV.
    mov r10, qword ptr [rsp]
    cmp r10, qword ptr [g_CutsceneCameraFOV_GameplayReturnAddr]
    je  SkipMultiplier
    cmp r10, qword ptr [g_CutsceneCameraFOV_PreviewReturnAddr]
    je  SkipMultiplier

    mov r10, qword ptr [g_CutsceneCameraFOV_MultiplierAddr]
    mulss xmm1, dword ptr [r10]

SkipMultiplier:
    push rbx
    sub rsp, 40h

    jmp qword ptr [g_CutsceneCameraFOV_ReturnAddr]

CutsceneCameraFOVDetour ENDP
END
