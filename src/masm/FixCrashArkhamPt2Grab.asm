; Detour1
.DATA
extern g_FixCrashArkhamPt2Grab_ReturnAddr1:QWORD

.CODE
FixCrashArkhamPt2GrabDetour1 PROC 
    cmp rcx, 80h
    je JmpOut
    jmp ActualCode

ActualCode:
    movups xmm0, [rcx]
    movaps xmm2,xmm0

JmpOut:
    jmp qword ptr [g_FixCrashArkhamPt2Grab_ReturnAddr1]

FixCrashArkhamPt2GrabDetour1 ENDP

; Detour2
.DATA
extern g_FixCrashArkhamPt2Grab_ReturnAddr2:QWORD

.CODE
FixCrashArkhamPt2GrabDetour2 PROC 
    cmp rcx, 80h
    je JmpOut
    jmp ActualCode

ActualCode:
    movups xmm0,[rcx]
    subps xmm0,xmm1

JmpOut:
    jmp qword ptr [g_FixCrashArkhamPt2Grab_ReturnAddr2]

FixCrashArkhamPt2GrabDetour2 ENDP

; Detour3
.DATA
extern g_FixCrashArkhamPt2Grab_ReturnAddr3:QWORD

.CODE
FixCrashArkhamPt2GrabDetour3 PROC 
    test rbx, rbx
    je JmpOut
    jmp ActualCode

ActualCode:
    addss xmm0, dword ptr [rbx+80h]

JmpOut:
    jmp qword ptr [g_FixCrashArkhamPt2Grab_ReturnAddr3]

FixCrashArkhamPt2GrabDetour3 ENDP

; Detour4
.DATA
extern g_FixCrashArkhamPt2Grab_ReturnAddr4:QWORD
.CODE
FixCrashArkhamPt2GrabDetour4 PROC 
    test rbx, rbx
    je JmpOut
    jmp ActualCode

ActualCode:
    addss xmm0, dword ptr [rbx+88h]

JmpOut:
    jmp qword ptr [g_FixCrashArkhamPt2Grab_ReturnAddr4]

FixCrashArkhamPt2GrabDetour4 ENDP

; Detour5
.DATA
extern g_FixCrashArkhamPt2Grab_ReturnAddr5:QWORD
.CODE
FixCrashArkhamPt2GrabDetour5 PROC 
    test rdx, rdx
    je JmpOut
    jmp ActualCode

ActualCode:
    movaps xmm0,[rdx+80h]

JmpOut:
    jmp qword ptr [g_FixCrashArkhamPt2Grab_ReturnAddr5]

FixCrashArkhamPt2GrabDetour5 ENDP
END
