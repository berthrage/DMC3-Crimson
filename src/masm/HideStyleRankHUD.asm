.CODE
extern g_HideStyleRankHUD_JumpAddr:QWORD
HideStyleRankHUDDetour PROC
    jmp jmpout

jmpout:
    jmp g_HideStyleRankHUD_JumpAddr

HideStyleRankHUDDetour ENDP
END
