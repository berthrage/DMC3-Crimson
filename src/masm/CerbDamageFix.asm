;CerbDamageFix
INCLUDE CommonMacros.inc
.DATA
extern g_CerbDamageFix_JmpAddr:QWORD
extern g_CerbDamageFix_ReturnAddr:QWORD
extern g_cerbDamageValue:DWORD

.CODE
CerbDamageFixDetour PROC
	mulss  xmm7, [g_cerbDamageValue]
    jmp OriginalCode



OriginalCode:
	comiss xmm6,xmm0
	jna JmpToJmpAddr
    jmp qword ptr [g_CerbDamageFix_ReturnAddr]

JmpToJmpAddr:
    jmp qword ptr [g_CerbDamageFix_JmpAddr]  

CerbDamageFixDetour ENDP
END