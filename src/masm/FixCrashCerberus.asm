.DATA
extern g_FixCrashCerberus_ReturnAddr:QWORD
extern g_FixCrashCerberus_PlayerStructAddr: QWORD

.CODE
FixCrashCerberusDetour PROC 
    test rax, rax
    jnz ContinueExecution
    mov r9, qword ptr [g_FixCrashCerberus_PlayerStructAddr]
    mov rax, [r9]
    jmp qword ptr [g_FixCrashCerberus_ReturnAddr]


ContinueExecution:   
    mov byte ptr [rax+05], 01 ; Original instruction
    mov r9, qword ptr [g_FixCrashCerberus_PlayerStructAddr]
    mov rax, [r9]
    ;mov rdx, [rax+18h]
    jmp qword ptr [g_FixCrashCerberus_ReturnAddr]


FixCrashCerberusDetour ENDP
END
