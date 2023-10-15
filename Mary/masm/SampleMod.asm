.CODE

extern g_SampleMod_ReturnAddr1:QWORD
SampleModDetour1 PROC
        jmp g_SampleMod_ReturnAddr1
SampleModDetour1 ENDP

extern g_SampleMod_ReturnAddr2:QWORD
SampleModDetour2 PROC
    jmp g_SampleMod_ReturnAddr2
SampleModDetour2 ENDP

END