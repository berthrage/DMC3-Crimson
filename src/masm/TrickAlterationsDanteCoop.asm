INCLUDE CommonMacros.inc
.DATA
extern g_DanteCoopTrick_ReturnAddr1:QWORD
extern g_DanteCoopTrick_coopTarget:DWORD
.CODE
DanteCoopTrickDetour1 PROC
	; player in RDI
	cmp byte ptr [rdi+3e00h], 48d ; ground trick event
	jne OriginalCode
	lea rdx,[g_DanteCoopTrick_coopTarget]
	jmp [g_DanteCoopTrick_ReturnAddr1] 

OriginalCode:
	lea rdx,[rdi+3FD0h]
	jmp [g_DanteCoopTrick_ReturnAddr1] 

DanteCoopTrickDetour1 ENDP

;

.DATA
extern g_DanteCoopTrick_ReturnAddr2:QWORD
.CODE
DanteCoopTrickDetour2 PROC
	; player in RDI
	cmp byte ptr [rdi+3e00h], 48d ; ground trick event
	jne OriginalCode
	lea rdx,[g_DanteCoopTrick_coopTarget]
	jmp [g_DanteCoopTrick_ReturnAddr2] 

OriginalCode:
	lea rdx,[rdi+3FD0h]
	jmp [g_DanteCoopTrick_ReturnAddr2] 

DanteCoopTrickDetour2 ENDP

;



.DATA
extern g_DanteCoopTrick_coordFunction:QWORD
extern g_DanteCoopTrick_ReturnAddr3:QWORD
.CODE
DanteCoopTrickDetour3 PROC
	; player in RDI
	cmp byte ptr [rdi+3e00h], 48d ; ground trick event
    jne OriginalCode
    mov rcx,rdi
    lea rdx,[g_DanteCoopTrick_coopTarget]
    call qword ptr [g_DanteCoopTrick_coordFunction]
    jmp [g_DanteCoopTrick_ReturnAddr3] 

OriginalCode:
	movzx eax,word ptr [rdi+6236h]
	jmp [g_DanteCoopTrick_ReturnAddr3] 

DanteCoopTrickDetour3 ENDP

;
END