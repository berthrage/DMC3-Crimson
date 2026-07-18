INCLUDE CommonMacros.inc

.DATA
EXTERN g_SwingRoseAirTauntInertiaAndSpawnShl_ReturnAddr:QWORD
EXTERN g_SwingRoseAirTauntInertiaAndSpawnShlCheckCall:QWORD       
EXTERN g_SwingRoseAirTauntInertiaAndSpawnShl_ActiveModelIndexCall:QWORD
EXTERN g_SwingRoseAirTauntInertiaAndSpawnShl_ShlSpawnCall:QWORD
newXinertiaMult dd -0.05f
xInertiaCap dd 10.0f
xInertiaDiv dd 1.15f
neil byte 0

.CODE
; From ExecuteCerberusSwingMove_sub_140206F70:
; dmc3.exe+2070B6 - F3 0F 10 90 CC 03 00 00 - movss xmm2,[rax+000003CC] { x inertia mult }
; dmc3.exe+2070BE - F3 0F 10 88 C8 03 00 00 - movss xmm1,[rax+000003C8] { x inertia }

SwingRoseAirTauntInertiaAndSpawnShlDetour PROC
; player in rbx & rcx

CheckIfInAirTauntRoseState:
    PushAllRegs
    add rsp, 28h
    call qword ptr [g_SwingRoseAirTauntInertiaAndSpawnShlCheckCall]  ; check if in Air Taunt Rose state
    sub rsp, 28h
    cmp al, 1
    jne OriginalCode
    PopAllRegs

SpawmCPl000Shl10e: ; (NevanShl, that's going to become Rose)
    PushAllRegs
    mov rdi, rcx
    add rsp, 28h
    call qword ptr [g_SwingRoseAirTauntInertiaAndSpawnShl_ActiveModelIndexCall]  
    sub rsp, 28h
    movzx r9d, byte ptr [rdi + 0B88Dh] 
    inc eax
    movzx r8d, byte ptr [rdi + 0C0h]
    mov rcx, rdi
    mov r8b, byte ptr [neil]
    mov [rsp + 28h], r8b
    mov rdx, [rdi + rax * 8 + 0E5D0h]
    movzx eax, byte ptr [rdi + 118h]
    mov r8b, byte ptr [neil]
    mov [rsp + 20h], r8b
    mov rdx, [rdx + 110h]
    call qword ptr [g_SwingRoseAirTauntInertiaAndSpawnShl_ShlSpawnCall]
    PopAllRegs

AdjustRoseThrowInertia:
    movss xmm2, dword ptr [newXinertiaMult]
    movss xmm5, dword ptr [rbx + 01C0h] ; load player current x inertia
    comiss xmm5, dword ptr [xInertiaCap] ; compare with x inertia cap
    jna DivideXInertia
    movss xmm5, dword ptr [xInertiaCap] ; if greater than cap, set to cap
    jmp DivideXInertia

DivideXInertia:
    divss xmm5, dword ptr [xInertiaDiv] ; divide by x inertia div
    movss xmm1, xmm5 
    jmp qword ptr [g_SwingRoseAirTauntInertiaAndSpawnShl_ReturnAddr]
    
OriginalCode:
    PopAllRegs
    movss xmm2, dword ptr [rax + 03CCh]
    movss xmm1, dword ptr [rax + 03C8h]
    jmp qword ptr [g_SwingRoseAirTauntInertiaAndSpawnShl_ReturnAddr]

SwingRoseAirTauntInertiaAndSpawnShlDetour ENDP

.DATA
EXTERN g_NevanShlMarkRoseMode_ReturnAddr:QWORD

.CODE
; From CPl000Shl10eNevanShlSetSpawn_sub_1401D6520:
; dmc3.exe+1D666C - 88 88 3C 04 00 00 - mov [rax+0000043C],cl
NevanShlMarkRoseModeDetour PROC

CheckIfInAirTauntRoseState:
    PushAllRegs
    add rsp, 28h
    mov rcx, rbx
    call qword ptr [g_SwingRoseAirTauntInertiaAndSpawnShlCheckCall]  ; check if in Air Taunt Rose state
    sub rsp, 28h
    cmp al, 1
    jne OriginalCode
    PopAllRegs
    mov [rax + 043Ch], cl 
    mov r8d, 1
    mov [rax + 0E0h], r8d ; mark as Rose mode
    jmp qword ptr [g_NevanShlMarkRoseMode_ReturnAddr]

OriginalCode:
    PopAllRegs
    mov [rax + 043Ch], cl
    jmp qword ptr [g_NevanShlMarkRoseMode_ReturnAddr]

NevanShlMarkRoseModeDetour ENDP
END