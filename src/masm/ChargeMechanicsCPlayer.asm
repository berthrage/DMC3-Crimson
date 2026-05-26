INCLUDE CommonMacros.inc

.DATA
EXTERN g_ChargeMechanicsCPlayer_ReturnAddr:QWORD
EXTERN g_ChargeMechanicsCPlayerCheckCall:QWORD
EXTERN g_ChargeMechanicsCPlayer_ConstAddr:QWORD
EXTERN g_FrameRateTimeMultiplier:DWORD
newChargeShotgunValue dd 100.0
chargeShotgunAddInc dd 1.0
chargeFormationAddInc dd 3.0


.CODE
; From DMC3_sub_1401EB3D0:
; dmc3.exe+1EB6F7 - 0F 2F 05 D2 BB 2D 00 - comiss xmm0,[dmc3.exe+4C72D0] { (10000.00) }
ChargeMechanicsCPlayerDetour PROC

CheckIfInChargeMechanic:
    PushAllRegs
    add rsp, 20h
    mov rcx, rdi ; player in rdi
    call qword ptr [g_ChargeMechanicsCPlayerCheckCall]
    cmp al, 1
    je ModifyChargeShotgun
    cmp al, 2
    je ModifySummonedSwordsFormationCharge
    jmp OriginalCode

ModifyChargeShotgun:
    sub rsp, 20h
    PopAllRegs
    movdqu [rsp], xmm3
    movss xmm3, dword ptr [chargeShotgunAddInc]
    jmp ContinueModifyChargeTime

ModifySummonedSwordsFormationCharge:
    sub rsp, 20h
    PopAllRegs
    movdqu [rsp], xmm3
    movss xmm3, dword ptr [chargeFormationAddInc]
    jmp ContinueModifyChargeTime

ContinueModifyChargeTime:
    mulss xmm3, dword ptr [g_FrameRateTimeMultiplier]
    addss xmm0, xmm3
    movdqu xmm3, [rsp]
    push rax
    mov rax, qword ptr [g_ChargeMechanicsCPlayer_ConstAddr]
    comiss xmm0, dword ptr [rax]
    pop rax
    jmp qword ptr [g_ChargeMechanicsCPlayer_ReturnAddr]

OriginalCode:
    sub rsp, 20h
    PopAllRegs
    push rax
    mov rax, qword ptr [g_ChargeMechanicsCPlayer_ConstAddr]
    comiss xmm0, dword ptr [rax]
    pop rax
    jmp qword ptr [g_ChargeMechanicsCPlayer_ReturnAddr]

ChargeMechanicsCPlayerDetour ENDP
END