INCLUDE CommonMacros.inc

.DATA
EXTERN g_FixEffectUVScroll_ReturnAddr:QWORD
EXTERN g_FixEffectUVScroll_StepCall:QWORD
EXTERN g_FixGeneratorSpawnTimer_ReturnAddr:QWORD

.CODE
; From CEffect_TickAnim_sub_1402E4970:
; dmc3.exe+2E4B2E - F3 0F 59 47 14       - mulss xmm0,[rdi+14] { scroll step truncated to whole units }
; dmc3.exe+2E4B4A - 41 FF 93 A8 000000   - call qword ptr [r11+000000A8] { CDraw add UV offset }
FixEffectUVScrollDetour PROC

    mov rcx, rdi ; effect, edx = mesh and r8d = group are still live from the original code
    call qword ptr [g_FixEffectUVScroll_StepCall]
    jmp qword ptr [g_FixEffectUVScroll_ReturnAddr]

FixEffectUVScrollDetour ENDP

; From CGenerator_Tick_sub_1402EBDC0:
; dmc3.exe+2EC1DC - F3 0F 11 87 0C010000 - movss [rdi+0000010C],xmm0 { spawn timer reset, drops the overshoot }
FixGeneratorSpawnTimerDetour PROC

    addss xmm0, dword ptr [rdi+10Ch] ; the timer still holds the (negative) overshoot of the tick that fired
    movss dword ptr [rdi+10Ch], xmm0
    jmp qword ptr [g_FixGeneratorSpawnTimer_ReturnAddr]

FixGeneratorSpawnTimerDetour ENDP

END
