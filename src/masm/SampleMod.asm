.DATA
    MsgBoxCaption  db "MASM MSGBOX",0 
    MsgBoxText     db "Hello from SampleMod.ASM",0 

.CODE
extern g_SampleMod_ReturnAddr1:QWORD
EXTERN MessageBoxA : PROTO
SampleModDetour1 PROC
    sub     rsp, 40 ; 00000028H
    xor     r9d, r9d
    lea     r8,  MsgBoxCaption
    lea     rdx, MsgBoxText
    xor     ecx, ecx
    call    MessageBoxA
    add     rsp, 40 ; 00000028H
    ret     0
SampleModDetour1 ENDP

;extern g_SampleMod_ReturnAddr2:QWORD
;SampleModDetour2 PROC
;    jmp g_SampleMod_ReturnAddr2
;SampleModDetour2 ENDP

END