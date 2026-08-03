INCLUDE CommonMacros.inc

.DATA
EXTERN g_MaxNevanShlDistance_ReturnAddr:QWORD
EXTERN g_MaxNevanShlDistanceCheckCall:QWORD           
EXTERN g_MaxNevanShlDistance_OgDistanceAddr:QWORD
newRange dd 100000.0f

.CODE
; From CPl000Shl10eNevanShlPreTrajectory_sub_1401D69C0:
; dmc3.exe+1D6AB2 - 0F 2F 05 5F F6 2E 00 - comiss xmm0,[dmc3.exe+4C6118] { (600.00) }
MaxNevanShlDistanceDetour PROC
; NevanShlActor in rbx
    cmp byte ptr [rbx + 0E0h], 1 ; check if RoseMode
    jne OriginalCode

IncreaseRange:
    comiss xmm0, dword ptr [newRange]
    jmp qword ptr [g_MaxNevanShlDistance_ReturnAddr]

OriginalCode:
    push rax
    mov rax, qword ptr [g_MaxNevanShlDistance_OgDistanceAddr]
    comiss xmm0, dword ptr [rax]
    pop rax
    jmp qword ptr [g_MaxNevanShlDistance_ReturnAddr]

MaxNevanShlDistanceDetour ENDP
END