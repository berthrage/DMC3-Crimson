#pragma once

struct ID3D11Resource;
struct ID3D11ShaderResourceView;
struct ID3D11Device;

struct D3D11Data {
    ID3D11Resource* texture {};
    ID3D11ShaderResourceView* textureView {};
};

struct DanteWeaponWheel {
    D3D11Data pistols;
    D3D11Data shotgun;
    D3D11Data laser;
    D3D11Data sniper;
    D3D11Data rocket;

    D3D11Data sword;
    D3D11Data nunchuks;
    D3D11Data dualswords;
    D3D11Data guitar;
    D3D11Data gauntlets;
};

struct VergilWeaponWheel {
    D3D11Data sword;
    D3D11Data katana;
    D3D11Data gauntlets;
};

struct WeaponWheelData {
    DanteWeaponWheel dw;
    VergilWeaponWheel vw;
};

WeaponWheelData* PacLoaderInitFileLoad(ID3D11Device* device);
