INCLUDE CommonMacros.inc

.DATA
EXTERN g_MenuAnimationStepCall:QWORD

.CODE
; Replaces ControlCFadeAmount_sub_140326960 entirely, so there is no original code to
; resume. The caller's return address is the only way to tell a menu panel step from a
; gameplay one, so it is handed to the C++ side in r8.
MenuAnimationStepDetour PROC

    mov r8, [rsp]
    jmp qword ptr [g_MenuAnimationStepCall]

MenuAnimationStepDetour ENDP

END
