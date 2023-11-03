.DATA
extern g_HoldToCrazyCombo_ReturnAddr:QWORD
extern g_holdToCrazyComboConditionalAddr:QWORD
extern holdToCrazyComboCall:QWORD

.CODE
HoldToCrazyComboDetour PROC ; player in RDI

    test word ptr [rdi+000074E0h], ax ; is melee held
    je JmpOutLabel

    push rax
    push rcx
    mov rcx, rdi
    call qword ptr [holdToCrazyComboCall]
    pop rcx

    cmp al, 1
    pop rax
    jne JmpOutLabel

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
