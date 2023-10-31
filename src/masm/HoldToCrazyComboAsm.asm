.DATA
    moveSwitch    dq 0
    stingerStart  dd  5.0f
    stingerEnd    dd 20.0f
    comboBStart   dd 69.0f
    comboBEnd     dd 70.0f

extern g_HoldToCrazyCombo_ReturnAddr:QWORD
extern g_holdToCrazyComboConditionalAddr:QWORD
extern MiaTimer:QWORD

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
    xor rax, rax
    xor rbx, rbx
    lea rax, [moveSwitch]
    mov rbx, [moveSwitch]
    movss xmm0, dword ptr [MiaTimer] ; current anim frame
    movss xmm1, dword ptr [rax+rbx] ; when can we cancel attack after
    comiss xmm0, xmm1
    pop rbx
    pop rax
    jb JmpOutLabel

    push rax
    push rbx
    xor rax, rax
    xor rbx, rbx
    lea rax, [moveSwitch]
    mov rbx, [moveSwitch]
    add rbx, 4
    movss xmm0, dword ptr [MiaTimer] ; current anim frame
    movss xmm1, dword ptr [rax + rbx] ; when can we cancel attack before
    comiss xmm0, xmm1
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