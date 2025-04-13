.DATA

.CODE
extern g_StyleRankHudNoFadeout_ReturnAddr:QWORD
StyleRankHudNoFadeoutDetour PROC
	mov [rbp+03D18h], r14d ;Updates the Style Rank HUD to display the decreased rank
	mov word ptr [rbp+03D20h], 01 ;Keeps Style Rank HUD On Screen insead of fading out
	jmp jmpout


fadeOut:
	mov word ptr [rbp+03D20h], 02
	jmp jmpout

jmpout:
	jmp g_StyleRankHudNoFadeout_ReturnAddr

StyleRankHudNoFadeoutDetour ENDP
END