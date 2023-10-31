.DATA
    lowerGuard dd 0.0f
    higherGuard dd 0.5f

.CODE
extern g_GuardGravity_ReturnAddr:QWORD
GuardGravityDetour PROC
    ; Original code, write air guard 1
    mov byte ptr [rbx+00006378h], 01h

    ; Test for held jump button
    cmp dword ptr [rbx+94h], 0
    jl jmpout
    test word ptr [rbx+74E0h], 0040h
    jne code2

    ; Jump not held
    movss xmm0, dword ptr [rbx+94h]
    mulss xmm0, lowerGuard
    movss dword ptr [rbx+94h], xmm0
    jmp jmpout

    ; Jump held
code2:
    movss xmm0, dword ptr [rbx+94h]
    mulss xmm0, higherGuard
    movss dword ptr [rbx+94h], xmm0
jmpout:
    jmp g_GuardGravity_ReturnAddr

GuardGravityDetour ENDP
END
