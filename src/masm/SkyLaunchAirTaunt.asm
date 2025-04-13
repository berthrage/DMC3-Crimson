.DATA
extern g_SkyLaunchForceRelease_ReturnAddr:QWORD
extern g_skyLaunchForceReleaseCheckCall:QWORD

.CODE
SkyLaunchForceReleaseDetour PROC
; This detour will trigger Force Just Frame Air Royal Release for either 
; Sky Launch or ToggleForceJustFrameRoyalRelease cheat
    push rax
    push rcx
    push rbx
    ; player is already in rcx (and rbx)
    ; Call the C++ function to check if we Force Royal Just Frame Air Release
    
    mov rcx, rbx ; Restore rcx as player since it might change due to air taunt's detour
    call qword ptr [g_skyLaunchForceReleaseCheckCall]
    cmp al, 01
    pop rbx
    pop rcx
    pop rax
    je ForceJustFrameAirRelease
    jmp OriginalCode

ForceJustFrameAirRelease:
    mov byte ptr [rbx + 00003FA4h], 195 ; Set Action to Just Frame Air Release
    
    jmp qword ptr [g_SkyLaunchForceRelease_ReturnAddr]

OriginalCode:
    ; Proceed normally
    mov byte ptr [rbx + 00003E10h], 01

    jmp qword ptr [g_SkyLaunchForceRelease_ReturnAddr]
    
SkyLaunchForceReleaseDetour ENDP

.DATA
extern g_SkyLaunchKillRGConsumption_ReturnAddr:QWORD
extern g_skyLaunchCheckCall:QWORD

.CODE
SkyLaunchKillRGConsumptionDetour PROC
; This detour will kill RG Gauge Consumption
; killing this will stop the consumption but will make Sky Launch into 
; a higher level Air Release, that's why we'll need to kill its Release Level.
    push rax
    push rcx
    push rbx
    ; player is already in rcx (and rbx)
    ; Call the C++ function to check if player is in SkyLaunch
    
    mov rcx, rbx ; Restore rcx as player since it might change due to air taunt's detour
    call qword ptr [g_skyLaunchCheckCall]
    cmp al, 01
    pop rbx
    pop rcx
    pop rax
    je KillRGGaugeConsumption
    jmp OriginalCode

KillRGGaugeConsumption:
    jmp qword ptr [g_SkyLaunchKillRGConsumption_ReturnAddr]

OriginalCode:
    mov qword ptr [rbx + 00006384h], 00000000
    jmp qword ptr [g_SkyLaunchKillRGConsumption_ReturnAddr]
    
SkyLaunchKillRGConsumptionDetour ENDP


.DATA
extern g_SkyLaunchKillReleaseLevel1_ReturnAddr:QWORD
extern g_skyLaunchCheckCall:QWORD

.CODE
SkyLaunchKillReleaseLevel1Detour PROC

    push rax
    push rcx
    push rbx
    ; you know the drill
    ; Call the C++ function to check if player is in SkyLaunch
    
    mov rcx, rbx ; Restore rcx as player since it might change due to air taunt's detour
    call qword ptr [g_skyLaunchCheckCall]
    cmp al, 01
    pop rbx
    pop rcx
    pop rax
    je KillReleaseLevel1
    jmp OriginalCode

KillReleaseLevel1:
    mov word ptr [rcx+00006380h], 0
    jmp qword ptr [g_SkyLaunchKillReleaseLevel1_ReturnAddr]

OriginalCode:
    mov word ptr [rcx+00006380h], 257
    jmp qword ptr [g_SkyLaunchKillReleaseLevel1_ReturnAddr]
    
SkyLaunchKillReleaseLevel1Detour ENDP

.DATA
extern g_SkyLaunchKillReleaseLevel2_ReturnAddr:QWORD
extern g_skyLaunchCheckCall:QWORD

.CODE
SkyLaunchKillReleaseLevel2Detour PROC

    push rax
    push rcx
    push rbx
    ; you know the drill
    ; Call the C++ function to check if player is in SkyLaunch
    
    mov rcx, rbx ; Restore rcx as player since it might change due to air taunt's detour
    call qword ptr [g_skyLaunchCheckCall]
    cmp al, 01
    pop rbx
    pop rcx
    pop rax
    je KillReleaseLevel2
    jmp OriginalCode

KillReleaseLevel2:
    mov word ptr [rcx+00006380h], 0
    jmp qword ptr [g_SkyLaunchKillReleaseLevel2_ReturnAddr]

OriginalCode:
    mov word ptr [rcx+00006380h], 770
    jmp qword ptr [g_SkyLaunchKillReleaseLevel2_ReturnAddr]
    
SkyLaunchKillReleaseLevel2Detour ENDP

.DATA
extern g_SkyLaunchKillReleaseLevel3_ReturnAddr:QWORD
extern g_skyLaunchCheckCall:QWORD

.CODE
SkyLaunchKillReleaseLevel3Detour PROC

    push rax
    push rcx
    push rbx
    ; you know the drill
    ; Call the C++ function to check if player is in SkyLaunch
    
    mov rcx, rbx ; Restore rcx as player since it might change due to air taunt's detour
    call qword ptr [g_skyLaunchCheckCall]
    cmp al, 01
    pop rbx
    pop rcx
    pop rax
    je KillReleaseLevel3
    jmp OriginalCode

KillReleaseLevel3:
    mov word ptr [rcx+00006380h], 0
    jmp qword ptr [g_SkyLaunchKillReleaseLevel3_ReturnAddr]

OriginalCode:
    mov word ptr [rcx+00006380h], 1283
    jmp qword ptr [g_SkyLaunchKillReleaseLevel3_ReturnAddr]
    
SkyLaunchKillReleaseLevel3Detour ENDP


.DATA
extern g_SkyLaunchKillDamage_ReturnAddr:QWORD
extern g_skyLaunchCheckCall:QWORD

align 16
saved_rdi dq 0.0

.CODE
SkyLaunchKillDamageDetour PROC
    ; player in rdi and r9 ( - 60h)
    push rax
    push rcx
    push rdx
    push rdi
    push r8
    push r9
    ; you know the drill
    ; Call the C++ function to check if player is in SkyLaunch
    
    mov qword ptr saved_rdi, rdi
    sub qword ptr saved_rdi, 60h
    mov rcx, qword ptr [saved_rdi] 
    call qword ptr [g_skyLaunchCheckCall]
    cmp al, 01
    pop r9
    pop r8
    pop rdi
    pop rdx
    pop rcx
    pop rax
    je KillDamage
    jmp OriginalCode

KillDamage:
    jmp qword ptr [g_SkyLaunchKillDamage_ReturnAddr]

OriginalCode:
    movss xmm9, dword ptr [rdx + 0Ch]
    jmp qword ptr [g_SkyLaunchKillDamage_ReturnAddr]
    
SkyLaunchKillDamageDetour ENDP


.DATA
extern g_SkyLaunchKillDamageCerberus_ReturnAddr:QWORD
extern g_skyLaunchCheckCall:QWORD

align 16
saved_rdi2 dq 0.0

.CODE
SkyLaunchKillDamageCerberusDetour PROC
    push rax
    push rcx
    push rdx
    push rdi
    push r8
    push r9

    ; you know the drill
    ; Call the C++ function to check if player is in SkyLaunch
    
    mov qword ptr saved_rdi2, rdi
    add qword ptr saved_rdi2, 20000h
    mov rcx, qword ptr [saved_rdi2] 
    call qword ptr [g_skyLaunchCheckCall]
    cmp al, 01
    pop r9
    pop r8
    pop rdi
    pop rdx
    pop rcx
    pop rax
    je KillDamageCerberus
    jmp OriginalCode

KillDamageCerberus:
    jmp qword ptr [g_SkyLaunchKillDamageCerberus_ReturnAddr]

OriginalCode:
    movss xmm7, dword ptr[rdx + 0Ch]
    jmp qword ptr [g_SkyLaunchKillDamageCerberus_ReturnAddr]
    
SkyLaunchKillDamageCerberusDetour ENDP


.DATA
extern g_SkyLaunchKillDamageShieldNevan_ReturnAddr:QWORD
extern g_skyLaunchCheckCall:QWORD

align 16 
saved_rbx dq 0.0

.CODE
SkyLaunchKillDamageShieldNevanDetour PROC
    ; player in rdi and rcx
    ; shielded nevan is in rbx, yes shielded nevan is a separate entity/actor from nevan
    push rcx
    push rbx
    push rdx
    ; you know the drill
    ; Call the C++ function to check if player is in SkyLaunch
   
    call qword ptr [g_skyLaunchCheckCall]
    cmp al, 01
    pop rdx
    pop rbx
    pop rcx
    je KillDamageShieldNevan
    jmp OriginalCode

KillDamageShieldNevan:
    jmp qword ptr [g_SkyLaunchKillDamageShieldNevan_ReturnAddr]

OriginalCode:
    movss dword ptr [rbx+00000224h], xmm0
    jmp qword ptr [g_SkyLaunchKillDamageShieldNevan_ReturnAddr]
    
SkyLaunchKillDamageShieldNevanDetour ENDP

END