.DATA
extern g_FixCrashVergilM3_ReturnAddr:QWORD
extern g_FixCrashVergilM3_JumpAddr:QWORD

.CODE
FixCrashVergilM3Detour PROC 
    test rax, rax
    je JmpOut
    cmp r12w,[rax]
    jl JmpToJmpAddr
    jmp JmpOut

JmpToJmpAddr:
    jmp g_FixCrashVergilM3_JumpAddr

JmpOut:
    jmp qword ptr [g_FixCrashVergilM3_ReturnAddr]

FixCrashVergilM3Detour ENDP
END
