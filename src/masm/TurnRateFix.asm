INCLUDE CommonMacros.inc
.DATA
EXTERN g_TurnRateFix_ReturnAddr:QWORD
EXTERN g_FasterTurnRateSettingAddr:QWORD
EXTERN g_FrameRateTimeMultiplier:DWORD
fasterTurnRateMultiplier dd 1.7

.CODE
TurnRateFixDetour PROC
    push rax
    sub rsp, 20h ; Reserving shadow-space for xmm
    movdqu [rsp], xmm0 ; Save xmm0
    movdqu [rsp + 10h], xmm1 ; Save xmm1

    ;mov rax, g_FrameRateTimeMultiplier
   ; movss xmm0, rax

    movsx eax, r9w ; r9w contains the original turn rate value (16-bit), move it to eax with sign extension
    cvtsi2ss xmm1, eax ; Convert the turn rate value to float in xmm1

    mulss xmm1, dword ptr [g_FrameRateTimeMultiplier] 

    mov rax, [g_FasterTurnRateSettingAddr] ; Get pointer to fasterTurnRate
    cmp byte ptr [rax], 1
    je TurnRateFasterFix
    jmp TurnRateFix


TurnRateFasterFix:
    mulss xmm1, [fasterTurnRateMultiplier]
    cvtss2si eax, xmm1 ; Convert the modified turn rate back to integer in eax
    mov r9w, ax
    jmp OriginalCode

TurnRateFix:
    cvtss2si eax, xmm1 
    mov r9w, ax
    jmp OriginalCode


OriginalCode:
    movsx ebx, r9w    ; Apply Turn Rate
    movdqu xmm0, [rsp]
    movdqu xmm1, [rsp + 10h]

    add rsp, 20h 
    pop rax
    movsx ecx, word ptr [r8]
    jmp qword ptr [g_TurnRateFix_ReturnAddr]


TurnRateFixDetour ENDP
END