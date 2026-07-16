INCLUDE CommonMacros.inc

.DATA
EXTERN g_FixWpNunchakuCerberusCollisions_ReturnAddr:QWORD
EXTERN g_FrameRateTimeMultiplier:DWORD

.CODE
; From WpNunchakuCollision_sub_14022F4C0:
; dmc3.exe+22FAFB - F3 0F 10 15 69 DA 12 00 - movss xmm2,[dmc3.exe+35D56C] { (1,00) }
FixWpNunchakuCerberusCollisionsDetour PROC

    movss xmm2, dword ptr [g_FrameRateTimeMultiplier]
    jmp qword ptr [g_FixWpNunchakuCerberusCollisions_ReturnAddr]

FixWpNunchakuCerberusCollisionsDetour ENDP
END