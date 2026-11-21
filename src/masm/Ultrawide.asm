.DATA
extern g_UltrawideCamera_ReturnAddr:QWORD
extern g_CullingFix_ReturnAddr:QWORD
extern g_UltrawideText_ReturnAddr:QWORD
extern g_UltrawideMovie_ReturnAddr:QWORD
extern g_UltrawideAspect:DWORD
extern g_UltrawideInvAspect:DWORD
extern g_CullingFixScale:DWORD
extern g_UltrawideCameraBase:DWORD
extern g_UltrawideCameraWritten:DWORD

movieAspect REAL4 1.7777778
half        REAL4 0.5

.CODE
UltrawideCameraDetour PROC
    ; rax = pRenderFrameState. +0x64 is written back, AdjustCameraProjection_sub_140325AD0 derives the focal length from it
    movss xmm9, dword ptr [rax+64h]
    ucomiss xmm9, dword ptr [g_UltrawideCameraWritten]
    je Scale
    movss dword ptr [g_UltrawideCameraBase], xmm9
Scale:
    movss xmm9, dword ptr [g_UltrawideCameraBase]
    mulss xmm9, dword ptr [g_UltrawideAspect]
    movss dword ptr [rax+64h], xmm9
    movss dword ptr [g_UltrawideCameraWritten], xmm9
    jmp qword ptr [g_UltrawideCamera_ReturnAddr]
UltrawideCameraDetour ENDP

CullingFixDetour PROC
    mulss xmm2, dword ptr [g_CullingFixScale]
    jmp qword ptr [g_CullingFix_ReturnAddr]
CullingFixDetour ENDP

UltrawideTextDetour PROC
    mulss xmm6, dword ptr [g_UltrawideInvAspect]
    mulss xmm0, xmm4
    cvtdq2ps xmm2, xmm2
    jmp qword ptr [g_UltrawideText_ReturnAddr]
UltrawideTextDetour ENDP

UltrawideMovieDetour PROC
    ; xmm1 = target width, rax = target height, rsp+30h = D3D11_VIEWPORT
    cvtsi2ss xmm0, rax
    movaps xmm2, xmm0
    mulss xmm2, dword ptr [movieAspect]
    comiss xmm1, xmm2
    jbe Store
    movaps xmm3, xmm1
    subss xmm3, xmm2
    mulss xmm3, dword ptr [half]
    movss dword ptr [rsp+30h], xmm3
    movaps xmm1, xmm2
Store:
    movss dword ptr [rsp+38h], xmm1
    movss dword ptr [rsp+3Ch], xmm0
    jmp qword ptr [g_UltrawideMovie_ReturnAddr]
UltrawideMovieDetour ENDP

END
