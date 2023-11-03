.DATA
extern g_HoldToCrazyCombo_ReturnAddr:QWORD
extern g_holdToCrazyComboConditionalAddr:QWORD
extern holdToCrazyComboActionTimer:QWORD
extern holdToCrazyComboCall:QWORD
extern holdToCrazyCombo_ShouldCC:BYTE

.CODE
HoldToCrazyComboDetour PROC ; player in RDI
    mov byte ptr [holdToCrazyCombo_ShouldCC], 0
    test word ptr [rdi+000074E0h], ax ; is melee held
    je JmpOutLabel

    mov rcx, rdi
    push rdi
    call qword ptr [holdToCrazyComboCall]
    pop rdi

    cmp byte ptr [holdToCrazyCombo_ShouldCC], 1
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
