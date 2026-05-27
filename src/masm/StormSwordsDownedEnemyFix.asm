INCLUDE CommonMacros.inc

.DATA
EXTERN g_StormSwordsDownedEnemyFix_ReturnAddr:QWORD
newStormSwordsRadius dd 50.0

.CODE
; From CPl021Shl01SummonedSword_StormSwords_SetPos_sub_1401D9DD0:
; dmc3.exe+1D9F2D - F3 0F 58 15 23 72 19 00 - addss xmm2,[dmc3.exe+371158] { 100.0f - Storm Swords radius around enemy }
StormSwordsDownedEnemyFixDetour PROC
    
    addss xmm2, dword ptr [newStormSwordsRadius]
    jmp qword ptr [g_StormSwordsDownedEnemyFix_ReturnAddr]

StormSwordsDownedEnemyFixDetour ENDP
END