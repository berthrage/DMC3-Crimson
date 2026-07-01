INCLUDE CommonMacros.inc

.DATA
EXTERN g_FixMPXinputVibration_ReturnAddr:QWORD
EXTERN g_FixMPXinputVibration_CallAddr:QWORD
EXTERN g_FixMPXinputVibrationCheckCall:QWORD        
EXTERN g_FrameRateTimeMultiplier:DWORD
EXTERN g_vibrationPlayerIndex:WORD
EXTERN g_currentVibrationIntensity:DWORD
EXTERN g_skipVibration:BYTE
vibrationDuration dd 0

.CODE
; From From PreXinputSendVibrationSignal_sub_140312190:
;    - E8 0E A6 01 00 - call dmc3.XinputSendVibrationSignal_sub_14032C7F0 { player in rsp+10h } { vibrationDuration in rsp+20h }
FixMPXinputVibrationDetour PROC
    PushAllRegs
    sub rsp, 20h
    mov rcx, qword ptr [rsp + 0E0h] ; player addr from caller's stack

CallVibrationPlayerIndexAssignment:
    call qword ptr [g_FixMPXinputVibrationCheckCall]
    add rsp, 20h

FixVibrationIntensity:
    xor rax, rax
    mov eax, [rsp + 70h] ; duration is passed in the caller's stack, so we need to get it from there
    mov dword ptr [vibrationDuration], eax
    PopAllRegs
    PushAllRegsAndXmm
    movss xmm0, dword ptr [vibrationDuration] 
    divss xmm0, dword ptr [g_FrameRateTimeMultiplier]
    mulss xmm0, dword ptr [g_currentVibrationIntensity]
    movss dword ptr [vibrationDuration], xmm0
    PopAllRegsAndXmm
    cmp byte ptr [g_skipVibration], 0
    jne SkipVibrationSignal
    mov r13d, dword ptr [vibrationDuration]
    mov [rsp + 20h], r13d

CallSendVibrationSignal:
    call qword ptr [g_FixMPXinputVibration_CallAddr]

SkipVibrationSignal:
    jmp qword ptr [g_FixMPXinputVibration_ReturnAddr]

FixMPXinputVibrationDetour ENDP
END