;StyleLevellingCCSFixDetour1
INCLUDE CommonMacros.inc
.DATA
extern g_StyleLevellingCCSFix_CheckCall1:QWORD
extern g_StyleLevellingCCSFix_ReturnAddr1:QWORD

.CODE
StyleLevellingCCSFixDetour1 PROC
    ; Player in RBX
    PushAllRegs
    mov rcx, rbx
    call [g_StyleLevellingCCSFix_CheckCall1]
    PopAllRegs
    jmp OriginalCode

OriginalCode:
    mov dword ptr [rbx+6358h], 1
    jmp qword ptr [g_StyleLevellingCCSFix_ReturnAddr1]


StyleLevellingCCSFixDetour1 ENDP

;StyleLevellingCCSFixDetour2
.DATA
extern g_StyleLevellingCCSFix_CheckCall2:QWORD
extern g_StyleLevellingCCSFix_ReturnAddr2:QWORD

.CODE
StyleLevellingCCSFixDetour2 PROC
    ; Player in RBX
    PushAllRegs
    mov rcx, rbx
    call [g_StyleLevellingCCSFix_CheckCall2]
    PopAllRegs
    jmp OriginalCode

OriginalCode:
    mov dword ptr [rbx+6358h], 2
    jmp qword ptr [g_StyleLevellingCCSFix_ReturnAddr2]


StyleLevellingCCSFixDetour2 ENDP

END

