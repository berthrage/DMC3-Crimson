INCLUDE CommonMacros.inc

.DATA
EXTERN g_FixSpiralRicochetRetrigger_ReturnAddr:QWORD

bounceGuard REAL4 4.0

.CODE
; From CPl000Shl04SpiralRicochet_sub_1401CE7E0:
; dmc3.exe+1CE8F1 - F3 0F 5C 46 14          - subss xmm0,[rsi+14]
; dmc3.exe+1CE8F6 - F3 0F 11 86 AC 01 00 00 - movss [rsi+000001AC],xmm0
; The bounce timer restarts at 5; ignore wall hits until a full 60fps tick has passed.
FixSpiralRicochetRetriggerDetour PROC

    subss xmm0, dword ptr [rsi+14h]
    movss dword ptr [rsi+1ACh], xmm0
    comiss xmm0, dword ptr [bounceGuard]
    jbe done
    and r8d, 0FFFCFFFFh
done:
    jmp qword ptr [g_FixSpiralRicochetRetrigger_ReturnAddr]

FixSpiralRicochetRetriggerDetour ENDP
END
