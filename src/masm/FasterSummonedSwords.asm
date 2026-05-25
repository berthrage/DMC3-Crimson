INCLUDE CommonMacros.inc

.DATA
EXTERN g_FasterSummonedSwords_ReturnAddr:QWORD
newSpd dd 2.0

.CODE
; From CPl021Shl01SummonedSwordUpdate_sub_1401DB140:
; dmc3.exe+1DB14E - 0F B6 53 08 - movzx edx,byte ptr [rbx+08]
; dmc3.exe+1DB152 - 85 D2                 - test edx,edx
FasterSummonedSwordsDetour PROC

CheckIfInBlisteringSwords:
    cmp byte ptr [rbx+09h], 3 ; summoned swords sword formation travel. Case 0: Normal, 1: Spiral Swords, 2: Storm Swords, 3: Blistering Swords
    je OriginalCode

ApplySpeed:
    movdqu [rsp], xmm1
    movdqu [rsp+10h], xmm2

    movss xmm1, dword ptr [newSpd]
    movss xmm2, dword ptr [rbx+014h] ; original speed, rbx holding CPl021Shl01SummonedSword's Actor Ptr
    mulss xmm2, xmm1 ; multiply by newSpd
    movss dword ptr [rbx+014h], xmm2 ; apply newSpd

    movdqu xmm2, [rsp+10h]
    movdqu xmm1, [rsp]

OriginalCode:
    movzx edx, byte ptr [rbx+08h]
    test edx, edx
    jmp qword ptr [g_FasterSummonedSwords_ReturnAddr]

FasterSummonedSwordsDetour ENDP
END