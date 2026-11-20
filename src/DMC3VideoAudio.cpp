#include "DMC3VideoAudio.hpp"

#include <windows.h>

#include "Core/Core.hpp"

namespace DMC3VideoAudio {

namespace {

// IXAudio2 2.7 vtable; the SDK's xaudio2.h is 2.8+ and lays it out differently.
constexpr uintptr_t XAUDIO2_RELEASE                = 0x10;
constexpr uintptr_t XAUDIO2_GET_DEVICE_COUNT       = 0x18;
constexpr uintptr_t XAUDIO2_GET_DEVICE_DETAILS     = 0x20;
constexpr uintptr_t XAUDIO2_CREATE_MASTERING_VOICE = 0x50;

// XAUDIO2_DEVICE_DETAILS: WCHAR DeviceID[256], WCHAR DisplayName[256], role, WAVEFORMATEXTENSIBLE.
constexpr size_t DEVICE_DETAILS_SIZE         = 0x800;
constexpr size_t DEVICE_DETAILS_DISPLAY_NAME = 0x200;

template <typename Ret, typename... Args>
Ret CallVirtual(void* object, uintptr_t offset, Args... args) {
    auto func = *reinterpret_cast<Ret(__fastcall**)(void*, Args...)>(*reinterpret_cast<byte8**>(object) + offset);
    return func(object, args...);
}

HRESULT TryDevice(void* xaudio2, void** voice, uint32 deviceIndex) {
    *voice = nullptr;
    HRESULT result = CallVirtual<HRESULT>(xaudio2, XAUDIO2_CREATE_MASTERING_VOICE, voice, uint32(0), uint32(0), uint32(0),
        deviceIndex, static_cast<void*>(nullptr));
    if (SUCCEEDED(result) && !*voice) {
        result = E_FAIL;
    }
    return result;
}

void LogFallbackDevice(void* xaudio2, uint32 deviceIndex, HRESULT defaultResult) {
    alignas(8) byte8 details[DEVICE_DETAILS_SIZE] = {};
    const wchar_t* name = L"?";
    if (SUCCEEDED(CallVirtual<HRESULT>(xaudio2, XAUDIO2_GET_DEVICE_DETAILS, deviceIndex, static_cast<void*>(details)))) {
        name = reinterpret_cast<const wchar_t*>(details + DEVICE_DETAILS_DISPLAY_NAME);
    }
    Log("Video audio: default output device failed (0x%08X), using device %u (%ls)", defaultResult, deviceIndex, name);
}

} // namespace

bool CreateMasteringVoice(PCFullMotionVideo& video) {
    void* xaudio2 = video.xaudio2;

    HRESULT defaultResult = TryDevice(xaudio2, &video.masteringVoice, 0);
    if (SUCCEEDED(defaultResult)) {
        return true;
    }

    // Device 0 is the default endpoint; the rest are every other active output.
    uint32 deviceCount = 0;
    CallVirtual<HRESULT>(xaudio2, XAUDIO2_GET_DEVICE_COUNT, &deviceCount);
    for (uint32 deviceIndex = 1; deviceIndex < deviceCount; ++deviceIndex) {
        if (SUCCEEDED(TryDevice(xaudio2, &video.masteringVoice, deviceIndex))) {
            LogFallbackDevice(xaudio2, deviceIndex, defaultResult);
            return true;
        }
    }

    Log("Video audio: no output device could be opened (0x%08X), skipping the video", defaultResult);
    CallVirtual<ULONG>(xaudio2, XAUDIO2_RELEASE);
    video.finished = true;
    return false;
}

} // namespace DMC3VideoAudio
