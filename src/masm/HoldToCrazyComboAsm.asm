.DATA
    moveSwitch    dq 0
    stingerStart  dd 0.10f ; high enough that 0 range stinger doesn't instantly million stab
    stingerEnd    dd 0.30f ; high enough that max range stinger still reaches CC frames, but low enough to still stinger->stinger
    comboBStart   dd 1.00f ; high enough that the CC transition is possible, even with hitstop from hitting multiple enemies
    comboBEnd     dd 1.01f ; low enough to transition into combo B ender

extern g_HoldToCrazyCombo_ReturnAddr:QWORD
extern g_holdToCrazyComboConditionalAddr:QWORD
extern holdToCrazyComboActionTimer:QWORD

.CODE
HoldToCrazyComboDetour PROC
    test word ptr [rdi+000074E0h], ax ; is melee held
    je JmpOutLabel

    cmp byte ptr [rdi+3fa4h], 7 ; unupgraded stinger
    je StingerLabel
    cmp byte ptr [rdi+3fa4h], 8 ; upgraded stinger
    je StingerLabel

    cmp byte ptr [rdi+3fa4h], 4
    je ComboBLabel

    jmp CodeLabel

StingerLabel:
    mov byte ptr [moveSwitch], 1*8
    jmp SetCrazyComboTimingLabel

ComboBLabel:
    mov byte ptr [moveSwitch], 2*8
    jmp SetCrazyComboTimingLabel

SetCrazyComboTimingLabel:
    push rax
    push rbx
    push rcx
    xor rax, rax
    xor rbx, rbx
    lea rax, [moveSwitch]
    mov rbx, [moveSwitch]
    mov rcx, [holdToCrazyComboActionTimer]
    movss xmm0, dword ptr [rcx] ; current anim frame
    movss xmm1, dword ptr [rax+rbx] ; when can we cancel attack after
    comiss xmm0, xmm1
    pop rcx
    pop rbx
    pop rax
    jb JmpOutLabel

    push rax
    push rbx
    push rcx
    xor rax, rax
    xor rbx, rbx
    lea rax, [moveSwitch]
    mov rbx, [moveSwitch]
    add rbx, 4
    mov rcx, [holdToCrazyComboActionTimer]
    movss xmm0, dword ptr [rcx] ; current anim frame
    movss xmm1, dword ptr [rax+rbx] ; when can we cancel attack after
    comiss xmm0, xmm1
    pop rcx
    pop rbx
    pop rax
    ja JmpOutLabel

    mov byte ptr [rsi+04],6
    jmp ReturnLabel

CodeLabel: ; modified originalcode for other moves
    test [rdi+000074E0h],ax
    je JmpOutLabel
    mov byte ptr [rsi+04], 6
ReturnLabel:
    jmp qword ptr [g_HoldToCrazyCombo_ReturnAddr]

JmpOutLabel:
    jmp qword ptr [g_holdToCrazyComboConditionalAddr]


HoldToCrazyComboDetour ENDP

END
