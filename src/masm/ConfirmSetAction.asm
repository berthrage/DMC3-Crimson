INCLUDE CommonMacros.inc
.DATA
extern g_ConfirmSetAction_ReturnAddr:QWORD
extern g_ConfirmSetAction_FuncCall:QWORD
extern g_ConfirmSetActionCheckCall:QWORD

.CODE
ConfirmSetActionDetour PROC
    PushAllXmm
    PushAllRegs
    sub rsp, 20h ; Reserving shadow-space for the call
    ; Call the C++ function to check if TriggerEvent with Action can occur
    call qword ptr [g_ConfirmSetActionCheckCall]
    add rsp, 20h       
    cmp al, 1
    je ContinueOriginalCode
    PopAllRegs
    PopAllXmm
    jmp qword ptr [g_ConfirmSetAction_ReturnAddr]

ContinueOriginalCode:
    PopAllRegs
    PopAllXmm
    call qword ptr [g_ConfirmSetAction_FuncCall] ; Call TriggerEvent with Action
    jmp qword ptr [g_ConfirmSetAction_ReturnAddr]
    
ConfirmSetActionDetour ENDP

END
