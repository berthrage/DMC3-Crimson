#pragma once

struct ID3D11Device;
typedef float ddVec3[3];

enum SsFxType {
    TRICK,
    SWORD,
    GUN,
    ROYAL,
    QUICK,
    DOPPEL,
    MAX
};

void InitStyleSwitchFxTexture(ID3D11Device* pd3dDevice);
void DrawStyleSwitchFxTexture();
void FreeStyleSwitchFxTexture();

void SetStyleSwitchFxWork(SsFxType sfxType, const float worldpos[3], const float color[3], float alpha, const float offset[2], float time, float size);
