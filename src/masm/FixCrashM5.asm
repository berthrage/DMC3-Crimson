;FixCrashM5Detour
INCLUDE CommonMacros.inc
.DATA
extern g_FixCrashM5_JmpAddr:QWORD
extern g_FixCrashM5_ReturnAddr:QWORD

.CODE
FixCrashM5Detour PROC
    test rdx, rdx
    je Jmpout
    jmp OriginalCode

Jmpout:
    jmp qword ptr [g_FixCrashM5_ReturnAddr]

OriginalCode:
    cmp [rdx+04h],eax
    jb JmpToJmpAddr
    jmp qword ptr [g_FixCrashM5_ReturnAddr]

JmpToJmpAddr:
    jmp qword ptr [g_FixCrashM5_JmpAddr]  

FixCrashM5Detour ENDP
END