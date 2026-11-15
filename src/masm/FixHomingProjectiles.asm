INCLUDE CommonMacros.inc

.DATA
EXTERN g_FixKalinaHystericAccel_ReturnAddr:QWORD
EXTERN g_FixSteerTurnScale_ReturnAddr:QWORD

oneTick REAL4 1.0

.CODE
; From KalinaHystericRocketFlight_sub_140172EC0:
; dmc3.exe+172F9D - F3 0F 10 0D 17 85 40 00 - movss xmm1,[dmc3.exe+57B4BC] { (1,00) }
FixKalinaHystericAccelDetour PROC

    movss xmm1, dword ptr [rdi+14h]
    jmp qword ptr [g_FixKalinaHystericAccel_ReturnAddr]

FixKalinaHystericAccelDetour ENDP

; From SteerMove_sub_140244870:
; dmc3.exe+2448C2 - 8B 81 80 01 00 00 - mov eax,[rcx+00000180]
; dmc3.exe+2448C8 - 0F 28 D3          - movaps xmm2,xmm3
; dmc3.exe+2448CB - 89 81 84 01 00 00 - mov [rcx+00000184],eax
; Turns are already gated by the interval timer, so the turn uses the time since the last turn
; in whole 60fps ticks instead of the per tick speed.
FixSteerTurnScaleDetour PROC

    movss xmm4, dword ptr [rcx+180h]
    movaps xmm2, xmm4
    subss xmm2, xmm1
    roundss xmm4, xmm4, 2
    maxss xmm4, dword ptr [oneTick]
    divss xmm2, xmm4
    mov eax, dword ptr [rcx+180h]
    mov dword ptr [rcx+184h], eax
    jmp qword ptr [g_FixSteerTurnScale_ReturnAddr]

FixSteerTurnScaleDetour ENDP
END
