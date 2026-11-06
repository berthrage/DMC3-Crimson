INCLUDE CommonMacros.inc
.DATA
extern g_ArtemisInstantCharge_FillCall:QWORD
extern g_ArtemisShotStartGround_ReturnAddr:QWORD
extern g_ArtemisShotStartAir_ReturnAddr:QWORD
extern g_ArtemisAirFireGate_Call:QWORD
extern g_ArtemisAirFireGate_ReturnAddr:QWORD
extern g_ArtemisAirFireGate_SkipAddr:QWORD
gateResult db 0

.CODE
; dmc3.exe+206AA8 - lea rcx,[rbx+41C0h] (ground Artemis shot, after the single lock acquire)
ArtemisShotStartGroundDetour PROC
    PushAllRegs
    sub rsp, 20h
    mov rcx, rbx
    call qword ptr [g_ArtemisInstantCharge_FillCall]
    add rsp, 20h
    PopAllRegs

    lea rcx, [rbx+41C0h]
    jmp qword ptr [g_ArtemisShotStartGround_ReturnAddr]
ArtemisShotStartGroundDetour ENDP

; dmc3.exe+206DF1 - lea rcx,[rbx+41C0h] (air Artemis shot)
ArtemisShotStartAirDetour PROC
    PushAllRegs
    sub rsp, 20h
    mov rcx, rbx
    call qword ptr [g_ArtemisInstantCharge_FillCall]
    add rsp, 20h
    PopAllRegs

    lea rcx, [rbx+41C0h]
    jmp qword ptr [g_ArtemisShotStartAir_ReturnAddr]
ArtemisShotStartAirDetour ENDP

; dmc3.exe+206CA4 - cmp byte ptr [rbx+3E2Ah],0 / jnz dmc3.exe+206F16 (air Artemis shot fire branch, both instructions replaced)
; The branch spawns the volley and applies the hover lift; it may only run once per shot start.
ArtemisAirFireGateDetour PROC
    cmp byte ptr [rbx+3E2Ah], 0
    jne SkipFire

    PushAllRegs
    sub rsp, 20h
    mov rcx, rbx
    call qword ptr [g_ArtemisAirFireGate_Call]
    add rsp, 20h
    mov byte ptr [gateResult], al
    PopAllRegs

    cmp byte ptr [gateResult], 0
    je SkipFire
    jmp qword ptr [g_ArtemisAirFireGate_ReturnAddr]

SkipFire:
    jmp qword ptr [g_ArtemisAirFireGate_SkipAddr]
ArtemisAirFireGateDetour ENDP

END