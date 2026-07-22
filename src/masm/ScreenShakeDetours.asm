INCLUDE CommonMacros.inc

.DATA
EXTERN g_ScreenShakeDetours_ReturnAddr:QWORD
EXTERN g_ScreenShakeDetoursCheckCall:QWORD   
EXTERN g_ScreenShakeDetours_ScreenShakeCallAddr:QWORD

.CODE
; From ManageWeaponMotionState_sub_1401F01F0:
; dmc3.exe+1F02F1 - E8 7A 56 13 00 - call dmc3.WeaponScreenShake_sub_140325970
ScreenShakeDetoursDetour PROC
; player in rbx
    PushAllRegs
    sub rsp, 28h
    mov rcx, rbx
    call qword ptr [g_ScreenShakeDetoursCheckCall]  ; check if in Air Taunt Rose state
    add rsp, 28h
    cmp al, 1
    jne OriginalCode
    PopAllRegs
    jmp qword ptr [g_ScreenShakeDetours_ReturnAddr]


OriginalCode:
    PopAllRegs
    call qword ptr [g_ScreenShakeDetours_ScreenShakeCallAddr]
    jmp qword ptr [g_ScreenShakeDetours_ReturnAddr]

ScreenShakeDetoursDetour ENDP
END