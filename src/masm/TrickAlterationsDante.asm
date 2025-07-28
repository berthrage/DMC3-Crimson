INCLUDE CommonMacros.inc
.DATA
extern g_DanteTrickAlter_ReturnAddr1:QWORD
newYInertiaVelocity dd -1500.0

.CODE
DanteTrickAlterationsDetour1 PROC
; AirTrickYInertia
	; player in RDI
	cmp byte ptr [rdi+3e00h], 48d ; ground trick event
	jne OriginalCode
	movss xmm6, dword ptr [newYInertiaVelocity] ; set new y inertia velocity
	jmp OriginalCode

OriginalCode:
	divss xmm6, dword ptr [rdi+3e34h]
	jmp [g_DanteTrickAlter_ReturnAddr1] 

DanteTrickAlterationsDetour1 ENDP

;

.DATA
extern g_DanteTrickAlter_ReturnAddr2:QWORD
newYInertiaVelocity2 dd -50.0

.CODE
DanteTrickAlterationsDetour2 PROC
; EndOfAirTrickYInertia
	; player in RBX
	cmp byte ptr [rbx+3e00h], 48d ; ground trick event
	jne OriginalCode
	movss xmm3, dword ptr [newYInertiaVelocity2] ; set new y inertia velocity
	jmp OriginalCode

OriginalCode:
    movss dword ptr [rbx+94h],xmm3
	jmp [g_DanteTrickAlter_ReturnAddr2] 

DanteTrickAlterationsDetour2 ENDP

;

.DATA
extern g_DanteTrickAlter_ReturnAddr3:QWORD

.CODE
DanteTrickAlterationsDetour3 PROC
; Trick landing anim so you input grounded moves just before you land
	; player in RBX
	cmp word ptr [rbx+3e08h], 18d ; only affect anim if trick was previous anim
	jne OriginalCode
	cmp byte ptr [rbx+3e00h], 48d ; ground trick event
	jne OriginalCode
	mov edx, 8h
	jmp [g_DanteTrickAlter_ReturnAddr3] 

OriginalCode:
	mov edx, 9h
	jmp [g_DanteTrickAlter_ReturnAddr3] 

DanteTrickAlterationsDetour3 ENDP

;

.DATA
extern g_DanteTrickAlter_ReturnAddr4:QWORD
EXTERN g_FrameRateTimeMultiplier:DWORD
one dd 1.0
newValue dd 2000.0

.CODE
DanteTrickAlterationsDetour4 PROC
; Increase Air Trick Horizontal Inertia (xmm0) & MaxAirTrickDistance (xmm8)
	; player in RDI
	cmp byte ptr [rdi+3e00h], 48d ; ground trick event
	jne OriginalCode
	jmp CalculateNewValue

CalculateNewValue:
	movss xmm11, dword ptr [newValue] 
	mulss xmm11, dword ptr [g_FrameRateTimeMultiplier] ; multiply by frame rate multiplier
	jmp CheckIfXInertiaAboveZero

CheckIfXInertiaAboveZero:
	movss xmm12, dword ptr [one]
	comiss xmm0, xmm12
	ja ModifyValues
	jmp OriginalCode

ModifyValues:
	movss xmm0, xmm11 ; set new x inertia
	movss xmm8, xmm11 ; set new max air trick distance
	jmp [g_DanteTrickAlter_ReturnAddr4] 

OriginalCode:
	movss xmm8, dword ptr [rbx+294h]
	jmp [g_DanteTrickAlter_ReturnAddr4] 

DanteTrickAlterationsDetour4 ENDP

END
