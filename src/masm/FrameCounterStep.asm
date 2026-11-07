INCLUDE CommonMacros.inc

.DATA
EXTERN g_frameCounterStep:DWORD
EXTERN g_frameCounterCarry:DWORD
EXTERN g_FrameCounterStep_ReturnAddr:QWORD

.CODE
; From FrameTick_sub_140337F00 (10 bytes):
; dmc3.exe+337F15 - FF 40 54             - inc dword ptr [rax+54h]
; dmc3.exe+337F18 - 48 8B 05 ...         - mov rax,cs:pRenderFrameState
; rax = pRenderFrameState, +0x54 = raw render frame counter. UI screens sample it for
; their blink and pulse phases, so it has to advance at 60 Hz instead of once per
; rendered frame. The step is fixed point (16.16) and the fraction is carried over.
FrameCounterStepDetour PROC

    mov ecx, dword ptr [g_frameCounterCarry]
    add ecx, dword ptr [g_frameCounterStep]
    mov dword ptr [g_frameCounterCarry], ecx
    shr ecx, 16
    and dword ptr [g_frameCounterCarry], 0FFFFh
    add dword ptr [rax+54h], ecx
    jmp qword ptr [g_FrameCounterStep_ReturnAddr]

FrameCounterStepDetour ENDP

END
