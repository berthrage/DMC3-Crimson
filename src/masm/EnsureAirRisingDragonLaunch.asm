.DATA
extern g_EnsureAirRisingDragonLaunch_ReturnAddr:QWORD
extern g_EnsureAirRisingDragonLaunch_JmpAddr:QWORD

.CODE
EnsureAirRisingDragonLaunchDetour PROC
    ; player in rbx
    push r13
    mov r13, [rbx+3e64h]
    test r13, 2
    pop r13
    je OriginalCode
    jmp g_EnsureAirRisingDragonLaunch_JmpAddr

OriginalCode:
    cmp byte ptr [rbx+3fa7h], 00      
    jmp qword ptr [g_EnsureAirRisingDragonLaunch_ReturnAddr]

EnsureAirRisingDragonLaunchDetour ENDP
END