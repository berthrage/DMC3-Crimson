INCLUDE CommonMacros.inc
.DATA
EXTERN g_InterceptCollisions_ReturnAddr:QWORD
EXTERN g_DrawCollisionsCall:QWORD
EXTERN g_InterceptCollisionsCall:QWORD

.CODE
InterceptCollisionsDetour PROC
	PushAllRegs
	sub		rsp, 20h ; Shadow space for the call
	mov rcx, rbx
	call qword ptr [g_InterceptCollisionsCall] ; do whatever you want with collisions
	add rsp, 20h
	movss xmm1, xmm0 ; copy float result
	PopAllRegs
	jmp DrawCollisions
	
DrawCollisions:
	PushAllRegsAndXmm
	mov rcx, rbx
	sub		rsp, 20h ; Shadow space for the call
	call qword ptr [g_DrawCollisionsCall] ; draw the collisions
	add rsp, 20h
	PopAllRegsAndXmm
	jmp OriginalCode

OriginalCode:
	movss dword ptr [rbx + 140h], xmm1
	jmp [g_InterceptCollisions_ReturnAddr]
InterceptCollisionsDetour ENDP

END