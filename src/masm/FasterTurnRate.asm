.DATA
extern g_FasterTurnRate_ReturnAddr:QWORD
extern g_FasterTurnRateCallAddr:QWORD
turnRateDivision dd 1000

.CODE
FasterTurnRateDetour PROC
    ; player in rbp
    call [g_FasterTurnRateCallAddr]
    movzx r9d, word ptr [rdi]
    jmp PerformDivision

PerformDivision:
    ; Move the value in AX to EAX (zero extend AX to EAX)
    movzx eax, ax

    push rcx
    push rdx
    ; Perform division 
    mov ecx, dword ptr [turnRateDivision]
    cdq                     ; Sign extend EAX into EDX:EAX
    idiv ecx                ; EAX = EAX / turnRateDivision, remainder in EDX
    pop rdx
    pop rcx

    ; After division, the quotient is in EAX
    jmp OriginalCode

OriginalCode:
    movzx r10d, ax         
    jmp qword ptr [g_FasterTurnRate_ReturnAddr]


FasterTurnRateDetour ENDP
END