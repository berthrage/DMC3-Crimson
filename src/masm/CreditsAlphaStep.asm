INCLUDE CommonMacros.inc

.DATA
EXTERN g_creditsAlphaStep:DWORD
EXTERN g_CreditsAlphaStep_ReturnAddr:QWORD

.CODE
; From CStaffRollUpdate_sub_1402454A0, credit text fade (9 bytes):
; dmc3.exe+245636 - 83 C0 02                - add eax,2
; dmc3.exe+245639 - 89 87 84 00 00 00       - mov [rdi+84h],eax
; rdi = CStaffRoll, +0x84 = text alpha (0..0x80). The step is rebuilt every frame so the
; fade takes the same time at any frame rate.
CreditsAlphaStepDetour PROC

    add eax, dword ptr [g_creditsAlphaStep]
    mov dword ptr [rdi+84h], eax
    jmp qword ptr [g_CreditsAlphaStep_ReturnAddr]

CreditsAlphaStepDetour ENDP

END
