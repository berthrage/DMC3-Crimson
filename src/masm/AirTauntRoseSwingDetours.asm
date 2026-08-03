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
    call qword ptr [g_SwingRoseAirTauntInertiaAndSpawnShl_ShlSpawnCall]  ; CPlNevanShlSpawnType1_sub_1402127F0 
                                                                         ; instead of CPl000Shl10eNevanShlSetSpawn_sub_1401D6520 
                                                                         ; so initialPos is consistent
    sub rsp, 28h
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

.DATA
EXTERN g_NevanShlSetToTravel_ReturnAddr:QWORD

.CODE
; From CPl000Shl10eNevanShlSetTrajectory_sub_1401D6E70:
; dmc3.exe+1D6E80 - 48 89 44 24 30 - mov [rsp+30],rax
NevanShlSetToTravelDetour PROC
    PushAllRegs
    mov rdx, [rcx + 01A0h] ; NevanShl's collision data start
    test rdx, rdx ; we check if collision has started before it's set to travel, 
                  ; otherwise it will be set to travel before collision sets in
    je OriginalCode
    cmp byte ptr [rcx + 0E0h], 1 ; check if Rose mode
    je SetToTravelForRose
    jmp OriginalCode

SetToTravelForRose:
    mov r8d, 2
    mov [rcx + 09h], r8d ; set Rose to Travel
    jmp OriginalCode

OriginalCode:
    PopAllRegs
    mov [rsp + 30h], rax
    jmp qword ptr [g_NevanShlSetToTravel_ReturnAddr]

NevanShlSetToTravelDetour ENDP

.DATA
EXTERN g_KillNevanRoseShlFX1_ReturnAddr:QWORD

.CODE
; From CPl000Shl10eNevanShlUpdate_sub_1401D6C00:
; dmc3.exe+1D6C28 - FF 90 E8 00 00 00 - call qword ptr [rax+000000E8]{ NevanShlTravelFX }
KillNevanRoseShlFX1Detour PROC
; NevanShlActor in rbx
CheckIfRoseShl:
    cmp byte ptr [rbx + 0E0h], 1 ; check if RoseMode
    jne OriginalCode
    jmp qword ptr [g_KillNevanRoseShlFX1_ReturnAddr]
    
OriginalCode:
    call qword ptr [rax + 0E8h] ; NevanShlTravelFX
    jmp qword ptr [g_KillNevanRoseShlFX1_ReturnAddr]

KillNevanRoseShlFX1Detour ENDP

.DATA
EXTERN g_KillNevanRoseShlFX2_ReturnAddr:QWORD
EXTERN g_KillNevanRoseShlFX2_SFXCall:QWORD 

.CODE
; From CPl000Shl10eNevanShlTravel_sub_1401D6C40:
; dmc3.exe+1D6DFC - E8 2F 2B 16 00 - call dmc3.PlaySFXWithPos_ByType_sub_140339930
KillNevanRoseShlFX2Detour PROC
; NevanShlActor in rdi
CheckIfRoseShl:
    cmp byte ptr [rdi + 0E0h], 1 ; check if RoseMode
    jne OriginalCode
    jmp qword ptr [g_KillNevanRoseShlFX2_ReturnAddr]

OriginalCode:
    call qword ptr [g_KillNevanRoseShlFX2_SFXCall]
    jmp qword ptr [g_KillNevanRoseShlFX2_ReturnAddr]

KillNevanRoseShlFX2Detour ENDP

.DATA
EXTERN g_KillNevanRoseShlEnemyTracking_ReturnAddr:QWORD
EXTERN g_KillNevanRoseShlEnemyTracking_JumpAddr:QWORD     

.CODE
; From CPl000Shl10eNevanShlTravel_sub_1401D6C40:
; dmc3.exe+1D6D74 - 80 BF 39 04 00 00 00 - cmp byte ptr [rdi+00000439],00 { 0 }
KillNevanRoseShlEnemyTrackingDetour PROC
; NevanShlActor in rdi
CheckIfRoseShl:
    cmp byte ptr [rdi + 0E0h], 1 ; check if RoseMode
    jne OriginalCode
    jmp g_KillNevanRoseShlEnemyTracking_JumpAddr

OriginalCode:
    cmp byte ptr [rdi + 0439h], 00
    jmp qword ptr [g_KillNevanRoseShlEnemyTracking_ReturnAddr]

KillNevanRoseShlEnemyTrackingDetour ENDP

.DATA
EXTERN g_FireNevanRoseShlVFX_ReturnAddr:QWORD
EXTERN g_NevanRoseShl_RoseEfkVFXCall:QWORD

.CODE
; From CPl000Shl10eNevanShlUpdate_sub_1401D6C00:
; dmc3.exe+1D6C06 - 0F B6 51 08 - movzx edx,byte ptr [rcx+08]
; dmc3.exe+1D6C0A - 48 8B D9 - mov rbx,rcx
FireNevanRoseShlVFXDetour PROC
; NevanShlActor in rcx
CheckIfRoseShl:
    cmp byte ptr [rcx + 0E0h], 1 ; check if RoseMode
    jne OriginalCode
    PushAllRegs
    sub rsp, 20h
    call qword ptr [g_NevanRoseShl_RoseEfkVFXCall]
    add rsp, 20h
    PopAllRegs

OriginalCode:
    movzx edx, byte ptr [rcx + 08h]
    mov rbx, rcx
    jmp qword ptr [g_FireNevanRoseShlVFX_ReturnAddr]

FireNevanRoseShlVFXDetour ENDP

.DATA
EXTERN g_FireNevanRoseShlDestroyFX_ReturnAddr:QWORD
EXTERN g_FireNevanRoseShlDestroyFX_OgCall:QWORD     
EXTERN g_NevanRoseShl_RoseEfkHitFXCall:QWORD
.CODE
; From CPl000Shl10eNevanShlPreTrajectory_sub_1401D69C0:
; dmc3.exe+1D6ABE - E8 4D FD FF FF - call dmc3.CPl000Shl10eNevanShlDestroyFX_sub_1401D6810
FireNevanRoseShlDestroyFXDetour PROC
; NevanShlActor in rcx
CheckIfRoseShl:
    cmp byte ptr [rcx + 0E0h], 1 ; check if RoseMode
    jne OriginalCode
    PushAllRegs
    sub rsp, 20h
    call qword ptr [g_NevanRoseShl_RoseEfkHitFXCall]
    add rsp, 20h
    PopAllRegs
    jmp qword ptr [g_FireNevanRoseShlDestroyFX_ReturnAddr]

OriginalCode:
    call qword ptr [g_FireNevanRoseShlDestroyFX_OgCall]
    jmp qword ptr [g_FireNevanRoseShlDestroyFX_ReturnAddr]

FireNevanRoseShlDestroyFXDetour ENDP
END