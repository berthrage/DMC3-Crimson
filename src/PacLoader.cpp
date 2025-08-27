#include "PacLoader.hpp"

#include <d3d11.h>
#include <cassert>
#include <cstdio>
#include "DDSTextureLoader.h"

static BOOL FileExistsA(LPCSTR filename) {
    DWORD attr = GetFileAttributesA(filename);
    return (attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY));
}

struct PacOffset {
    ptrdiff_t offset;
    size_t    size;
};

struct DantePacOffsets {

    PacOffset pistols     { .offset = 356896, .size = 8320 };
    PacOffset shotgun     { .offset = 369632, .size = 8320 };
    PacOffset laser       { .offset = 382368, .size = 8320 };
    PacOffset sniper      { .offset = 395104, .size = 8320 };
    PacOffset rocket      { .offset = 407840, .size = 8320 };

    PacOffset sword       { .offset = 433312, .size = 8320 };
    PacOffset nunchuks    { .offset = 446048, .size = 8320 };
    PacOffset dualswords  { .offset = 458784, .size = 8320 };
    PacOffset guitar      { .offset = 471520, .size = 8320 };
    PacOffset gauntlets   { .offset = 484256, .size = 8320 };

};

struct VergilPacOffsets {

    PacOffset sword       { .offset = 356896, .size = 8320 };
    PacOffset katana      { .offset = 433312, .size = 8320 };
    PacOffset gauntlets   { .offset = 446048, .size = 8320 };
};


static constexpr DantePacOffsets danteOffsets {};
static constexpr VergilPacOffsets vergilOffsets {};

static WeaponWheelData wheel {};

static void load(ID3D11Device* dev, byte* pacfile, const PacOffset& o, D3D11Data* result) {

    HRESULT res = 
        DirectX::CreateDDSTextureFromMemory(dev, pacfile + o.offset, o.size, &result->texture, &result->textureView);
    if (FAILED(res)) {
        assert(NULL && "CreateDDSTextureFromMemory failed!");
        result->texture = nullptr;
        result->textureView = nullptr;
    }
}

struct ReadFileResult
{
    uint32_t ContentsSize;
    void *Contents;
};

static ReadFileResult
ReadEntireFile(const char *FileName)
{
    ReadFileResult Result = {};

    FILE *File = fopen(FileName, "rb");
    if (File)
    {
        fseek(File, 0, SEEK_END);
        Result.ContentsSize = (uint32_t)ftell(File);
        fseek(File, 0, SEEK_SET);

        // Allocate extra byte for null terminator
        Result.Contents = malloc(Result.ContentsSize + 1);
        if (Result.Contents)
        {
            fread(Result.Contents, Result.ContentsSize, 1, File);

            // Null terminate
            ((uint8_t *)Result.Contents)[Result.ContentsSize] = 0;
        }
        else
        {
            // Handle malloc failure
            Result.ContentsSize = 0;
        }

        fclose(File);
    }

    return Result;
}

WeaponWheelData* PacLoaderInitFileLoad(ID3D11Device* device) {
    assert(device);
    static const char* id100filename = ".\\data\\dmc3\\GData.afs\\id100.pac";
    static const char* id100vfilename = ".\\data\\dmc3\\GData.afs\\id100V.pac";

    if (!FileExistsA(id100filename)) {
        assert(NULL && "Error id100.pac file does not exist");
        return nullptr;
    }
    if (!FileExistsA(id100vfilename)) {
        assert(NULL && "Error id100V.pac file does not exist");
        return nullptr;
    }
    ReadFileResult f1 = ReadEntireFile(id100filename);
    assert(f1.ContentsSize);
    ReadFileResult f2 = ReadEntireFile(id100vfilename);
    assert(f2.ContentsSize);
    byte* pacId100 = (byte*)f1.Contents;
    byte* pacId100v = (byte*)f2.Contents;

    load(device, pacId100, danteOffsets.pistols,     &wheel.dw.pistols);
    load(device, pacId100, danteOffsets.shotgun,     &wheel.dw.shotgun);
    load(device, pacId100, danteOffsets.laser,       &wheel.dw.laser);
    load(device, pacId100, danteOffsets.sniper,      &wheel.dw.sniper);
    load(device, pacId100, danteOffsets.rocket,      &wheel.dw.rocket);
    load(device, pacId100, danteOffsets.sword,       &wheel.dw.sword);
    load(device, pacId100, danteOffsets.nunchuks,    &wheel.dw.nunchuks);
    load(device, pacId100, danteOffsets.dualswords,  &wheel.dw.dualswords);
    load(device, pacId100, danteOffsets.guitar,      &wheel.dw.guitar);
    load(device, pacId100, danteOffsets.gauntlets,   &wheel.dw.gauntlets);

    load(device, pacId100v, vergilOffsets.sword,     &wheel.vw.sword);
    load(device, pacId100v, vergilOffsets.katana,    &wheel.vw.katana);
    load(device, pacId100v, vergilOffsets.gauntlets, &wheel.vw.gauntlets);

    return &wheel;
}