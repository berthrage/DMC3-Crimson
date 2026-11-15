INCLUDE CommonMacros.inc

.DATA
EXTERN g_FixShotgunShlCollision_ReturnAddr1:QWORD
EXTERN g_FixShotgunShlCollision_ReturnAddr2:QWORD
EXTERN g_FixGunShlCollision_ReturnAddr1:QWORD
EXTERN g_FixGunShlCollision_ReturnAddr2:QWORD

oneTick REAL4 1.0

.CODE
; From CPl000Shl01ShotgunUpdate_sub_1401CA7D0:
; dmc3.exe+1CA80D - F3 0F 10 53 14 - movss xmm2,[rbx+14]
FixShotgunShlCollisionDetour1 PROC

    movss xmm2, dword ptr [oneTick]
    jmp qword ptr [g_FixShotgunShlCollision_ReturnAddr1]

FixShotgunShlCollisionDetour1 ENDP

; dmc3.exe+1CAD23 - F3 0F 5C 43 14 - subss xmm0,[rbx+14]
FixShotgunShlCollisionDetour2 PROC

    subss xmm0, dword ptr [oneTick]
    jmp qword ptr [g_FixShotgunShlCollision_ReturnAddr2]

FixShotgunShlCollisionDetour2 ENDP

; From CPl000Shl00GunShotUpdate_sub_1401C9D00:
; dmc3.exe+1C9D48 - F3 0F 10 57 14 - movss xmm2,[rdi+14]
FixGunShlCollisionDetour1 PROC

    movss xmm2, dword ptr [oneTick]
    jmp qword ptr [g_FixGunShlCollision_ReturnAddr1]

FixGunShlCollisionDetour1 ENDP

; dmc3.exe+1CA04A - F3 0F 5C 47 14 - subss xmm0,[rdi+14]
FixGunShlCollisionDetour2 PROC

    subss xmm0, dword ptr [oneTick]
    jmp qword ptr [g_FixGunShlCollision_ReturnAddr2]

FixGunShlCollisionDetour2 ENDP
END
