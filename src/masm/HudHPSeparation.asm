.DATA
    newSeparationValue dd 8.5f

.CODE
extern g_HudHPSeparation_ReturnAddr:QWORD
HudHPSeparationDetour PROC
    subss xmm0, newSeparationValue ;[dmc3.exe+4E9080h] ;original value is 12.92
    jmp jmpout

    
jmpout:
    jmp g_HudHPSeparation_ReturnAddr

HudHPSeparationDetour ENDP
END
