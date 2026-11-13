INCLUDE CommonMacros.inc

.DATA
EXTERN g_FrameRateTimeMultiplier:DWORD

EXTERN g_FixLockOnCamRelease_ReturnAddr1:QWORD
EXTERN g_FixLockOnCamRelease_ReturnAddr2:QWORD
EXTERN g_FixLockOnCamRelease_ReturnAddr3:QWORD

releaseMoveThreshold dd 3.0f    ; units per frame the player has to move to count a frame of the release
releaseFrames dd 10.0f          ; frames of the release at 60 FPS
releaseSpeedCap dd 15.0f        ; per-frame easing cap right after the release, at 60 FPS

.CODE
FixLockOnCamReleaseDetour1 PROC

    ; xmm0 = distance moved since the last camera update, xmm1 is dead here
    movss xmm1, dword ptr [releaseMoveThreshold]
    mulss xmm1, dword ptr [g_FrameRateTimeMultiplier]
    comiss xmm0, xmm1

    jmp qword ptr [g_FixLockOnCamRelease_ReturnAddr1]

FixLockOnCamReleaseDetour1 ENDP


.CODE
FixLockOnCamReleaseDetour2 PROC

    ; rbx is the camera. rax and xmm0 are free, the next instruction overwrites both.
    movss xmm0, dword ptr [releaseFrames]
    divss xmm0, dword ptr [g_FrameRateTimeMultiplier]
    cvttss2si eax, xmm0
    cmp eax, 1
    jge ClampMax
    mov eax, 10                                 ; multiplier not measured yet, keep the vanilla count
    jmp StoreCount

ClampMax:
    cmp eax, 127                                ; the counter is a signed byte
    jle StoreCount
    mov eax, 127

StoreCount:
    mov byte ptr [rbx+1ECh], al                 ; release frames left
    mov byte ptr [rbx+1EDh], 0                  ; has not moved far enough yet

    jmp qword ptr [g_FixLockOnCamRelease_ReturnAddr2]

FixLockOnCamReleaseDetour2 ENDP


.CODE
FixLockOnCamReleaseDetour3 PROC

    ; rbx is the camera, xmm0 is dead here (the function returns void)
    movss xmm0, dword ptr [releaseSpeedCap]
    mulss xmm0, dword ptr [g_FrameRateTimeMultiplier]
    movss dword ptr [rbx+1E4h], xmm0            ; eye easing cap
    movss dword ptr [rbx+1E8h], xmm0            ; lookat easing cap

    jmp qword ptr [g_FixLockOnCamRelease_ReturnAddr3]

FixLockOnCamReleaseDetour3 ENDP

END
