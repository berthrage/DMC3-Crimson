#include "StyleSwitchFX.hpp"
#include "UI/Texture2DD3D11.hpp"
#include "glm/glm.hpp"
#include <array>
#include "DebugDrawDX11.hpp"
#include "CrimsonFileHandling.hpp"

#define TEXTURE_ATLAS_FILENAME "stylenames.png"

static Texture2DD3D11* g_StyleSwitchAtlasTexture { nullptr };

struct SsFxCtx {
    SsFxType sfxType;
    ddVec3 pos;
    ddVec3 offset;
    ddVec3 color;
    float alpha;
    float time;
    float size;
};

static SsFxCtx g_StyleSwitchFxWork {};

struct Rect {
    glm::vec2 m_pos;
    glm::vec2 m_size;

    constexpr explicit Rect(float x, float y, float w, float h) :
        m_pos(x, y), m_size(w, h) {};

    constexpr glm::vec2 rmin() const {
        return m_pos;
    }
    constexpr glm::vec2 rmax() const {
        return glm::vec2(m_pos.x + m_size.y, m_pos.y + m_size.y);
    }
};

struct TextureAtlas {
    static constexpr glm::vec2 m_AtlasSize { 2048.0f, 2048.0f };
    std::array<Rect, 8> rects = {
    //      pos.x    pos.y   size.x  size.y
    Rect { 114.0f,  111.0f,  627.0f, 257.0f }, //trick  ;
    Rect { 1218.0f, 85.0f,   709.0f, 250.0f }, //sword  ;
    Rect { 217.0f,  449.0f,  426.0f, 238.0f }, //gun    ;
    Rect { 1255.0f, 446.0f,  688.0f, 258.0f }, //royal  ;
    Rect { 1287.0f, 838.0f,  620.0f, 251.0f }, //quick  ;
    Rect { 106.0f,  794.0f,  831.0f, 240.0f }, //doppel ;
    Rect { 285.0f,  1138.0f, 285.0f, 244.0f }, //dt     ;
    Rect { 1279.0f, 1138.0f, 634.0f, 253.0f }, //dte    ;
    };

    constexpr glm::vec2 uv0(const Rect& rect) const { 
        return glm::vec2(
            rect.m_pos.x / m_AtlasSize.x, 
            rect.m_pos.y / m_AtlasSize.y); 
    };
    constexpr glm::vec2 uv1(const Rect& rect) const {
        return glm::vec2(
            (rect.m_pos.x + rect.m_size.x) / m_AtlasSize.x,
            (rect.m_pos.y + rect.m_size.y) / m_AtlasSize.y);
    };
};
static constexpr TextureAtlas g_TextureAtlas{};

void InitStyleSwitchFxTexture(ID3D11Device* pd3dDevice) {
#ifndef _NDEBUG
    char buffer[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, buffer);
    OutputDebugStringA(buffer);
#endif
    g_StyleSwitchAtlasTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + TEXTURE_ATLAS_FILENAME).c_str(), pd3dDevice);
    assert(g_StyleSwitchAtlasTexture);
}

void DrawStyleSwitchFxTexture() {
    assert(g_StyleSwitchAtlasTexture);
    if (!g_StyleSwitchAtlasTexture->IsValid()) {
        return;
    }

    if (g_StyleSwitchFxWork.time <= 0.0f) {
        return;
    }

    ImGuiIO& io = ImGui::GetIO();
    g_StyleSwitchFxWork.time -= io.DeltaTime;

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMouseInputs;


    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(io.DisplaySize);

    ImGui::Begin("Overlay", nullptr, window_flags); {
        const float factor = g_StyleSwitchFxWork.size;
        ImDrawList* drawlist = ImGui::GetWindowDrawList();
        const Rect& rect = g_TextureAtlas.rects[g_StyleSwitchFxWork.sfxType];

        glm::vec2 uv0 = g_TextureAtlas.uv0(rect);
        glm::vec2 uv1 = g_TextureAtlas.uv1(rect);
        

        const float ar = rect.m_size.x / rect.m_size.y;
        
        float xSize = rect.m_size.x * factor / ar;
        float ySize = rect.m_size.y * factor / ar;

        float xScreenPos = g_StyleSwitchFxWork.pos[0] - (xSize / 2.0f) + g_StyleSwitchFxWork.offset[0];
        float yScreenPos = g_StyleSwitchFxWork.pos[1] - (ySize / 2.0f) + g_StyleSwitchFxWork.offset[1];

        ImVec2 pmin(xScreenPos, yScreenPos);
        ImVec2 pmax((pmin.x + xSize), (pmin.y + ySize));
        ImVec2 uv00(uv0.x, uv0.y);
        ImVec2 uv01(uv1.x, uv1.y);

        auto color = ImColor::ImColor(
            g_StyleSwitchFxWork.color[0],
            g_StyleSwitchFxWork.color[1],
            g_StyleSwitchFxWork.color[2],
            g_StyleSwitchFxWork.alpha - g_StyleSwitchFxWork.time);

        drawlist->AddImage(g_StyleSwitchAtlasTexture->GetTexture(), pmin, pmax, uv00, uv01, color);
    }
    ImGui::End();

}

void FreeStyleSwitchFxTexture() {
    delete g_StyleSwitchAtlasTexture;
}

void SetStyleSwitchFxWork(SsFxType sfxType, const float worldpos[3], const float color[3], float alpha, const float offset[2], float time, float size) {
    ZeroMemory(&g_StyleSwitchFxWork, sizeof(g_StyleSwitchFxWork));
    g_StyleSwitchFxWork.sfxType = sfxType;
    SimpleVec3 screenpos = debug_draw_world_to_screen(worldpos, size);
    memcpy(g_StyleSwitchFxWork.pos, &screenpos, sizeof(ddVec3));
    memcpy(g_StyleSwitchFxWork.color, color, sizeof(ddVec3));
    g_StyleSwitchFxWork.alpha = alpha;
    if (offset) {
        memcpy(g_StyleSwitchFxWork.offset, offset, sizeof(float) * 2);
    }
    g_StyleSwitchFxWork.time = time;
    g_StyleSwitchFxWork.size = size;
}
