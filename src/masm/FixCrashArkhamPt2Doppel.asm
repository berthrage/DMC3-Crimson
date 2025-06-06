; Detour1
.DATA
extern g_FixCrashArkhamPt2Doppel_ReturnAddr1:QWORD
extern g_FixCrashArkhamPt2Doppel_CallAddr1:QWORD

.CODE
FixCrashArkhamPt2DoppelDetour1 PROC 
    push rdx
    mov rdx, 100CA8918h ; bad address check
    cmp rax, rdx
    pop rdx
    je JmpOut
    jmp ActualCode

ActualCode:
    add rcx,[rax]
    call [g_FixCrashArkhamPt2Doppel_CallAddr1]

JmpOut:
    jmp qword ptr [g_FixCrashArkhamPt2Doppel_ReturnAddr1]

FixCrashArkhamPt2DoppelDetour1 ENDP

; Detour2
.DATA
extern g_FixCrashArkhamPt2Doppel_ReturnAddr2:QWORD
extern g_FixCrashArkhamPt2Doppel_CallAddr2:QWORD

.CODE
FixCrashArkhamPt2DoppelDetour2 PROC 
    push rdx
    mov rdx, 100CA8918h ; bad address check
    cmp rax, rdx
    pop rdx
    je JmpOut
    jmp ActualCode

ActualCode:
    dec qword ptr [rax+20h]
    call [g_FixCrashArkhamPt2Doppel_CallAddr2]

JmpOut:
    jmp qword ptr [g_FixCrashArkhamPt2Doppel_ReturnAddr2]

FixCrashArkhamPt2DoppelDetour2 ENDP
END
