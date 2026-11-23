INCLUDE CommonMacros.inc

.DATA
EXTERN g_KillWeaponMotionEffect_ReturnAddr:QWORD
EXTERN g_KillWeaponMotionEffectCheckCall:QWORD

.CODE
; From ManageWeaponMotionState_sub_1401F01F0:
; dmc3.exe+1F027D - 88 83 103A0000 - mov [rbx+00003A10],al { actor.weaponEffect }
KillWeaponMotionEffectDetour PROC
; player in rbx
    PushAllRegs
    mov rcx, rbx
    sub rsp, 28h
    call qword ptr [g_KillWeaponMotionEffectCheckCall]
    add rsp, 28h
    cmp al, 1
    jne OriginalCode
    PopAllRegs
    mov byte ptr [rbx + 03A10h], 0
    jmp qword ptr [g_KillWeaponMotionEffect_ReturnAddr]

OriginalCode:
    PopAllRegs
    mov [rbx + 03A10h], al
    jmp qword ptr [g_KillWeaponMotionEffect_ReturnAddr]

KillWeaponMotionEffectDetour ENDP
END
