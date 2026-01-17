INCLUDE CommonMacros.inc
.DATA
extern g_PlaytimeOnTick_ReturnAddr:QWORD
extern g_TriggerOnTickFuncsCall:QWORD
extern g_PlaytimeOnTickMovAddr:QWORD

.CODE
PlaytimeOnTickDetour PROC
    imul ecx,eax, 00057E40h
    PushAllXmm
    PushAllRegs
    sub rsp, 20h ; Reserving shadow-space for the call
    ; Call the C++ function to trigger all OnTick Functions
    call qword ptr [g_TriggerOnTickFuncsCall]
    add rsp, 20h       

    jmp ContinueOriginalCode

ContinueOriginalCode:
    PopAllRegs
    PopAllXmm
    
    jmp qword ptr [g_PlaytimeOnTick_ReturnAddr]
    
PlaytimeOnTickDetour ENDP

END
