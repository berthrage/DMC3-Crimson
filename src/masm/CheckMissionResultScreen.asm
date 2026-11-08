; From CUIDResultUpdate_sub_1402A3170:
; dmc3.exe+2A2718 - E8 B3 00 00 00 - call dmc3.sub_1402A27D0 { begin mission results screen }
; dmc3.exe+2A2768 - E8 B30C0000           - call dmc3.sub_1402A3420 { process mission results screen }
; dmc3.exe+2A277C - E8 5F0C0000           - call dmc3.sub_1402A33E0 { process save screen }
; dmc3.exe+2A2772 - E8 E91A0000           - call dmc3.sub_1402A4260 { leaving mission results }

INCLUDE CommonMacros.inc

.DATA
EXTERN g_CheckMissionResultScreen_ReturnAddr1:QWORD
EXTERN g_CheckMissionResultScreen_ReturnAddr2:QWORD
EXTERN g_CheckMissionResultScreen_ReturnAddr3:QWORD
EXTERN g_CheckMissionResultScreen_ReturnAddr4:QWORD
EXTERN g_CheckMissionResultScreen_TargetAddr1:QWORD
EXTERN g_CheckMissionResultScreen_TargetAddr2:QWORD
EXTERN g_CheckMissionResultScreen_TargetAddr3:QWORD
EXTERN g_CheckMissionResultScreen_TargetAddr4:QWORD
EXTERN appBaseAddr:QWORD
EXTERN g_inMissionResultScreen:WORD
EXTERN g_ApplyMultiplayerStylishPtsAverageCall:QWORD

.CODE
CheckMissionResultScreenDetour1 PROC
    mov word ptr [g_inMissionResultScreen], 1
    mov rax, qword ptr [g_ApplyMultiplayerStylishPtsAverageCall]
    test rax, rax
    jz SkipApplyStylishPtsAverage
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 32
    call rax
    add rsp, 32
    pop r9
    pop r8
    pop rdx
    pop rcx
SkipApplyStylishPtsAverage:
    call qword ptr [g_CheckMissionResultScreen_TargetAddr1]
    jmp qword ptr [g_CheckMissionResultScreen_ReturnAddr1]
CheckMissionResultScreenDetour1 ENDP

.CODE
CheckMissionResultScreenDetour2 PROC
    mov word ptr [g_inMissionResultScreen], 1
    call qword ptr [g_CheckMissionResultScreen_TargetAddr2]
    jmp qword ptr [g_CheckMissionResultScreen_ReturnAddr2]
CheckMissionResultScreenDetour2 ENDP

.CODE
CheckMissionResultScreenDetour3 PROC
    mov word ptr [g_inMissionResultScreen], 0
    call qword ptr [g_CheckMissionResultScreen_TargetAddr3]
    jmp qword ptr [g_CheckMissionResultScreen_ReturnAddr3]
CheckMissionResultScreenDetour3 ENDP

.CODE
CheckMissionResultScreenDetour4 PROC
    mov word ptr [g_inMissionResultScreen], 0
    call qword ptr [g_CheckMissionResultScreen_TargetAddr4]
    jmp qword ptr [g_CheckMissionResultScreen_ReturnAddr4]
CheckMissionResultScreenDetour4 ENDP
END