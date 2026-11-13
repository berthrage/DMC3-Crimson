INCLUDE CommonMacros.inc

.DATA
EXTERN g_FixCStageSetSeal_ReturnAddr1:QWORD
EXTERN g_FixCStageSetSeal_ReturnAddr2:QWORD
EXTERN g_FixCStageSetSeal_ReturnAddr3:QWORD
EXTERN g_FixCStageSetSeal_ReturnAddr4:QWORD
EXTERN g_FixCStageSetSeal_FadeStepCall:QWORD
constOne dd 1.0

.CODE
; From CStageSetSealHandAndAnim_sub_14026E910:
; dmc3.exe+26E9BD - F3 44 0F 2C C0 - cvttss2si r8d,xmm0 { fade in step truncation (bad) }
FixCStageSetSealDetour1 PROC

    push rcx
    push rdx
    mov rdx, rbx ; seal object
    sub rsp, 20h
    call qword ptr [g_FixCStageSetSeal_FadeStepCall] ; xmm0 = step, returns whole step with the fraction carried over
    add rsp, 20h
    pop rdx
    pop rcx
    mov r8d, eax
    jmp qword ptr [g_FixCStageSetSeal_ReturnAddr1]

FixCStageSetSealDetour1 ENDP
; dmc3.exe+26E984 - F3 44 0F 2C C0 - cvttss2si r8d,xmm0 { fade out step truncation (bad) }
FixCStageSetSealDetour4 PROC

    push rcx
    push rdx
    mov rdx, rbx ; seal object
    sub rsp, 20h
    call qword ptr [g_FixCStageSetSeal_FadeStepCall]
    add rsp, 20h
    pop rdx
    pop rcx
    mov r8d, eax
    jmp qword ptr [g_FixCStageSetSeal_ReturnAddr4]

FixCStageSetSealDetour4 ENDP
; dmc3.exe+26EB09 - F3 0F 10 73 14 - movss xmm6,[rbx+14] { setting speed for scrolling anim }
FixCStageSetSealDetour2 PROC

    movss xmm6, dword ptr [constOne] ; set to 1.0 to prevent slowdown of scrolling anim
    jmp qword ptr [g_FixCStageSetSeal_ReturnAddr2]

FixCStageSetSealDetour2 ENDP
; dmc3.exe+26EB46 - F3 44 0F 2C C8 - cvttss2si r9d,xmm0 { scrolling speed truncation (bad) }
; dmc3.exe+26EB4B - F3 44 0F 2C C1 - cvttss2si r8d,xmm1
FixCStageSetSealDetour3 PROC

    cvtss2si r9d, xmm0 ; cvttss2si to cvtss2si to combat aggressive rounding
    cvtss2si r8d, xmm1
    jmp qword ptr [g_FixCStageSetSeal_ReturnAddr3]

FixCStageSetSealDetour3 ENDP
END