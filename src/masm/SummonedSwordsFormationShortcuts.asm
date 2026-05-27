INCLUDE CommonMacros.inc

.DATA
EXTERN g_SummonedSwordsFormationShortcuts_ReturnAddr:QWORD
EXTERN g_SummonedSwordsFormationShortcutsCheckCall:QWORD 

holdInput dword 0

.CODE
; From CPl021Shl01SummonedSword_SpiralSwordsUpdate_sub_1401D9770:
; dmc3.exe+1D98B5 - 8B 82 D8 B5 00 00 - mov eax,[rdx+0000B5D8] { Checking player queued SwordFormation state }
SummonedSwordsFormationShortcutsDetour PROC

    push rbx
    push rcx
    push rdx

    sub rsp,28h

    ; rcx = player
    ; rdx = shl
    mov rcx, rdx
    mov rdx, rdi
    call g_SummonedSwordsFormationShortcutsCheckCall

 
    movzx eax, al                ; zero-extend uint8 return to 32-bit for cmp eax,1 at 0x1D98BB
    ; eax = shortcut result (0=none, 1=Storm, 2=Blistering)
    ; Original code at dmc3.exe+1D98BB handles targeting copy

    add rsp,28h

    pop rdx
    pop rcx
    pop rbx

    jmp qword ptr [g_SummonedSwordsFormationShortcuts_ReturnAddr]

SummonedSwordsFormationShortcutsDetour ENDP
END