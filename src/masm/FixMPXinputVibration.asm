INCLUDE CommonMacros.inc

.DATA
EXTERN g_FixMPXinputVibration_ReturnAddr:QWORD
EXTERN g_FixMPXinputVibration_CallAddr:QWORD
EXTERN g_FixMPXinputVibrationCheckCall:QWORD           

.CODE
; From From PreXinputSendVibrationSignal_sub_140312190:
; dmc3.exe+3121DD - E8 0EA60100           - call dmc3.XinputSendVibrationSignal_sub_14032C7F0 { player in rsp+10 } 
FixMPXinputVibrationDetour PROC
    PushAllRegs
    sub rsp, 20h
    mov rcx, qword ptr [rsp + 0E0h] ; player addr from caller's stack

CallVibrationPlayerIndexAssignment:
    call qword ptr [g_FixMPXinputVibrationCheckCall]
    add rsp, 20h
    PopAllRegs
    call qword ptr [g_FixMPXinputVibration_CallAddr]
    jmp qword ptr [g_FixMPXinputVibration_ReturnAddr]

AltPlayerFetch:
    test rbp, rbp
    je JmpOut
    mov rcx, qword ptr [rbp + 4Fh]
    jmp CallVibrationPlayerIndexAssignment

JmpOut:
    mov rcx, rbp
    jmp CallVibrationPlayerIndexAssignment

FixMPXinputVibrationDetour ENDP
END