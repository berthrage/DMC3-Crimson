.DATA
EXTERN g_FixSecretMissionTimerFPS_ReturnAddr:QWORD
EXTERN g_FrameRateTimeMultiplierRounded:DWORD

.CODE
FixSecretMissionTimerFPSDetour PROC 
    divss xmm0, dword ptr [g_FrameRateTimeMultiplierRounded] ; Divide the timer by the frame rate multiplier
    jmp OriginalCode ; 
    
OriginalCode:
    movss dword ptr [rcx+6948h], xmm0
    jmp    qword ptr [g_FixSecretMissionTimerFPS_ReturnAddr]

FixSecretMissionTimerFPSDetour ENDP

.DATA
EXTERN g_FixSecretMissionTimerFPS_ReturnAddr2:QWORD
EXTERN g_FrameRateTimeMultiplier:DWORD

.CODE
FixSecretMissionTimerFPSDetour2 PROC 
    divss xmm0, dword ptr [g_FrameRateTimeMultiplier] ; Here we can use "FrameRateTimeMultiplier" for accuracy since game is loaded and running
    jmp OriginalCode 
    
OriginalCode:
    subss xmm1, xmm0
    xorps xmm0, xmm0
    jmp    qword ptr [g_FixSecretMissionTimerFPS_ReturnAddr2]

FixSecretMissionTimerFPSDetour2 ENDP
END