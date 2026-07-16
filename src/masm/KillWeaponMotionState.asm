INCLUDE CommonMacros.inc

.DATA
EXTERN g_KillWeaponMotionState_ReturnAddr:QWORD
EXTERN g_KillWeaponMotionStateCheckCall:QWORD           

.CODE
; From ManageWeaponMotionState_sub_1401F01F0:
; dmc3.exe+1F0277 - 88 8B C3 39 00 00 - mov [rbx+000039C3],cl { Update Weapon Motion State 3 (+0x39C3) Makes Swords go to hand }
KillWeaponMotionStateDetour PROC
; player in rbx
    PushAllRegs
    mov rcx, rbx
    sub rsp, 28h
    call qword ptr [g_KillWeaponMotionStateCheckCall]
    add rsp, 28h
    cmp al, 1
    jne OriginalCode
    PopAllRegs
    jmp qword ptr [g_KillWeaponMotionState_ReturnAddr]
    

OriginalCode:
    PopAllRegs
    mov [rbx + 039C3h], cl
    jmp qword ptr [g_KillWeaponMotionState_ReturnAddr]

KillWeaponMotionStateDetour ENDP
END