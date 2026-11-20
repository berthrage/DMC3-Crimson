.DATA
extern g_FixVideoAudioDevice_ReturnAddr:QWORD
extern g_FixVideoAudioDevice_FailAddr:QWORD
extern g_FixVideoAudioDevice_Call:QWORD

.CODE
FixVideoAudioDeviceDetour PROC
    ; rdi = PCFullMotionVideo, rbx = &video->xaudio2, rbp = 0
    ; Everything volatile is dead after the replaced CreateMasteringVoice call
    push rbp
    mov rbp, rsp
    and rsp, -16
    sub rsp, 20h
    mov rcx, rdi
    call qword ptr [g_FixVideoAudioDevice_Call] ; DMC3VideoAudio::CreateMasteringVoice
    mov rsp, rbp
    pop rbp
    test al, al
    jz NoDevice
    jmp qword ptr [g_FixVideoAudioDevice_ReturnAddr]

NoDevice:
    jmp qword ptr [g_FixVideoAudioDevice_FailAddr] ; nulls video->xaudio2 and returns

FixVideoAudioDeviceDetour ENDP

END
