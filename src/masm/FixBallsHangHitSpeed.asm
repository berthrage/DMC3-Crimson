.DATA
EXTERN g_FixBallsHangHitSpeed_ReturnAddr:QWORD
EXTERN g_FrameRateTimeMultiplier:DWORD
Half REAL4 0.5

.CODE
FixBallsHangHitSpeedDetour PROC 
    sub    rsp, 20h                  ; Allocate 32 bytes for xmm0 and xmm1
    movdqu [rsp], xmm0               ; Save xmm0 at [rsp]
    movdqu [rsp+10h], xmm1           ; Save xmm1 at [rsp+16]

   movss xmm3, dword ptr [rsi-4Ch]
   movss xmm0, dword ptr [g_FrameRateTimeMultiplier]
   mulss xmm0, xmm0                ; xmm0 = x^2
   movss xmm1, dword ptr [g_FrameRateTimeMultiplier]
   sqrtss xmm1, xmm1               ; xmm1 = sqrt(x)
   mulss xmm0, xmm1                ; xmm0 = x^2 * sqrt(x) = x^2.5
   divss xmm3, xmm0                ; xmm3 /= x^2.5

    movdqu xmm0, [rsp]               ; Restore xmm0
    movdqu xmm1, [rsp+10h]           ; Restore xmm1
    add    rsp, 20h                  

    jmp    qword ptr [g_FixBallsHangHitSpeed_ReturnAddr]
FixBallsHangHitSpeedDetour ENDP
END