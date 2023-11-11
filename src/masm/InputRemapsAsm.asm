.DATA
extern g_DTRemap_ReturnAddr:QWORD
extern g_DTRemap_NewMap:WORD
.CODE
DTRemapDetour PROC
  cmp dword ptr [rbx+78h], 3 ; vergil
  jne CodeLabel
  movzx eax,word ptr [g_DTRemap_NewMap]
  jmp ReturnLabel
CodeLabel:
    movzx eax, word ptr [rbp+rax*2+0Ah]
ReturnLabel:
    jmp qword ptr [g_DTRemap_ReturnAddr]

DTRemapDetour ENDP

.DATA
extern g_ShootRemap_ReturnAddr:QWORD
extern g_ShootRemap_NewMap:WORD
.CODE
ShootRemapDetour PROC
  cmp dword ptr [rbx+78h], 3 ; vergil
  jne CodeLabel
  movzx eax,word ptr [g_ShootRemap_NewMap]
  jmp ReturnLabel
CodeLabel:
    movzx eax, word ptr [rbp+rax*2+0Ah]
ReturnLabel:
    jmp qword ptr [g_ShootRemap_ReturnAddr]

ShootRemapDetour ENDP

END
