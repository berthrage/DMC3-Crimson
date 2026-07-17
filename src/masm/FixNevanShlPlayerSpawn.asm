INCLUDE CommonMacros.inc

.DATA
EXTERN g_FixNevanShlPlayerSpawn_ReturnAddr1:QWORD
EXTERN g_FixNevanShlPlayerSpawn_ReturnAddr2:QWORD

.CODE
; From CPl000Shl10eNevanShlSetInitialPos_sub_1401D66A0:
; dmc3.exe+1D670A - 48 8B 7C C8 18 - mov rdi,[rax+rcx*8+18] { load player 1 actor for initialPos }
FixNevanShlPlayerSpawnDetour1 PROC
    test rdi, rdi
    je AltCode
    mov rdi, [rdi + 04C0h]
    jmp qword ptr [g_FixNevanShlPlayerSpawn_ReturnAddr1]

AltCode:
    mov rdi, [rbx + 04C0h]
    jmp qword ptr [g_FixNevanShlPlayerSpawn_ReturnAddr1]

FixNevanShlPlayerSpawnDetour1 ENDP
; From CPl000Shl10eNevanShlTravel_sub_1401D6C40:
; dmc3.exe+1D6C89 - 48 8B 5C D0 18 - mov rbx,[rax+rdx*8+18] { load player 1 actor for travelPos }
FixNevanShlPlayerSpawnDetour2 PROC
    mov rbx, [rcx + 04C0h]
    jmp qword ptr [g_FixNevanShlPlayerSpawn_ReturnAddr2]

FixNevanShlPlayerSpawnDetour2 ENDP
END