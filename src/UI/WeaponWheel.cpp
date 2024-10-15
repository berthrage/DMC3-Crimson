#include "WeaponWheel.hpp"

#include "Sprite.hpp"

#include <filesystem>

std::vector<Graphics::Sprite> g_Sprites;

enum class TextureID 
{
    ColoredDantePanel1Active = 0,
    ColoredDantePanel2Active,
    ColoredDantePanel3Active,
    ColoredDantePanel4Active,
    ColoredDantePanel5Active,

    ColoredDantePanel1Inactive,
    ColoredDantePanel2Inactive,
    ColoredDantePanel3Inactive,
    ColoredDantePanel4Inactive,
    ColoredDantePanel5Inactive,

    ColoredDanteArrow1,
    ColoredDanteArrow2,
    ColoredDanteArrow3,
    ColoredDanteArrow4,
    ColoredDanteArrow5,
    ColoredDanteCenter,

    ColoredVergilPanel1Active,
    ColoredVergilPanel2Active,
    ColoredVergilPanel3Active,
    ColoredVergilPanel4Active,
    ColoredVergilPanel5Active,

    ColoredVergilPanel1Inactive,
    ColoredVergilPanel2Inactive,
    ColoredVergilPanel3Inactive,
    ColoredVergilPanel4Inactive,
    ColoredVergilPanel5Inactive,

    ColoredVergilArrow1,
    ColoredVergilArrow2,
    ColoredVergilArrow3,
    ColoredVergilArrow4,
    ColoredVergilArrow5,
    ColoredVergilCenter,

    ColorlessPanel1Active,
    ColorlessPanel2Active,
    ColorlessPanel3Active,
    ColorlessPanel4Active,
    ColorlessPanel5Active,

    ColorlessPanel1Inactive,
    ColorlessPanel2Inactive,
    ColorlessPanel3Inactive,
    ColorlessPanel4Inactive,
    ColorlessPanel5Inactive,

    ColorlessArrow1,
    ColorlessArrow2,
    ColorlessArrow3,
    ColorlessArrow4,
    ColorlessArrow5,
    ColorlessCenter,

    RebellionAwakenedActive,
    RebellionDormantActive,
    CerberusActive,
    AgniRudraActive,
    NevanActive,
    BeowulfActive,
    ForceEdgeActive,

    EbonyIvoryActive,
    ShotgunActive,
    ArtemisActive,
    SpiralActive,
    KalinaActive,

    YamatoActive,
    YamatoForceEdgeActive,

    DuplicateRebellionAwakenedActive,
    DuplicateRebellionDormantActive,
    DuplicateCerberusActive,
    DuplicateAgniRudraActive,
    DuplicateNevanActive,
    DuplicateBeowulfActive,
    DuplicateForceEdgeActive,

    DuplicateEbonyIvoryActive,
    DuplicateShotgunActive,
    DuplicateArtemisActive,
    DuplicateSpiralActive,
    DuplicateKalinaActive,

    DuplicateYamatoActive,
    DuplicateYamatoForceEdgeActive,

    RebellionAwakenedInactive,
    RebellionDormantInactive,
    CerberusInactive,
    AgniRudraInactive,
    NevanInactive,
    BeowulfInactive,
    ForceEdgeInactive,

    EbonyIvoryInactive,
    ShotgunInactive,
    ArtemisInactive,
    SpiralInactive,
    KalinaInactive,

    YamatoInactive,
    YamatoForceEdgeInactive,

    Size
};

struct Transform
{
    glm::vec3 Translation;
    glm::vec3 Rotation;
    glm::vec3 Scale;
};

static constexpr Transform s_MeleeInactiveSlotTransforms[5] =
{
    { // Slot 1
        glm::vec3(-0.142f, 0.416f, 0.0f),
        glm::vec3(0.0f, 0.0f, glm::radians(55.120f)),
        glm::vec3(0.244f)
    },
    { // Slot 2
        glm::vec3(0.416f, 0.362f, 0.0f),
        glm::vec3(0.0f, 0.0f, glm::radians(-12.919f)),
        glm::vec3(0.227f)
    },
    { // Slot 3
        glm::vec3(0.384f, -0.330f, 0.0f),
        glm::vec3(0.0f, 0.0f, glm::radians(27.560f)),
        glm::vec3(0.178f)
    },
    { // Slot 4
        glm::vec3(-0.086f, -0.504f, 0.0f),
        glm::vec3(0.0f, 0.0f, glm::radians(80.366f)),
        glm::vec3(0.217f)
    },
    { // Slot 5
        glm::vec3(-0.524f, -0.016f, 0.0f),
        glm::vec3(0.0f, 0.0f, glm::radians(23.254f)),
        glm::vec3(0.214f)
    }
};

static constexpr Transform s_MeleeActiveSlotTransforms[5] =
{
    { // Slot 1
        glm::vec3(-0.278f, 0.546f, 0.0f),
        glm::vec3(0.0f, 0.0f, glm::radians(49.831f)),
        glm::vec3(0.446f)
    },
    { // Slot 2
        glm::vec3(0.522f, 0.448f, 0.0f),
        glm::vec3(0.0f, 0.0f, glm::radians(-11.186f)),
        glm::vec3(0.420f)
    },
    { // Slot 3
        glm::vec3(0.472f, -0.424f, 0.0f),
        glm::vec3(0.0f, 0.0f, glm::radians(23.390f)),
        glm::vec3(0.388f)
    },
    { // Slot 4
        glm::vec3(-0.122f, -0.540f, 0.0f),
        glm::vec3(0.0f, 0.0f, glm::radians(78.486f)),
        glm::vec3(0.391f)
    },
    { // Slot 5
        glm::vec3(-0.594f, 0.012f, 0.0f),
        glm::vec3(0.0f, 0.0f, glm::radians(23.254f)),
        glm::vec3(0.359f)
    }
};

namespace WW
{
    void LoadSprites()
    {
        g_Sprites.reserve((size_t)TextureID::Size);
    
        const std::filesystem::path textureRoot(R"(Crimson\assets\weaponwheel)");
    
        g_Sprites.emplace_back((textureRoot / R"(colored\dante\panelsactive\panel1.png)").string().c_str()); //ColoredDantePanel1Active
        g_Sprites.emplace_back((textureRoot / R"(colored\dante\panelsactive\panel2.png)").string().c_str()); //ColoredDantePanel2Active
        g_Sprites.emplace_back((textureRoot / R"(colored\dante\panelsactive\panel3.png)").string().c_str()); //ColoredDantePanel3Active
        g_Sprites.emplace_back((textureRoot / R"(colored\dante\panelsactive\panel4.png)").string().c_str()); //ColoredDantePanel4Active
        g_Sprites.emplace_back((textureRoot / R"(colored\dante\panelsactive\panel5.png)").string().c_str()); //ColoredDantePanel5Active
    
        g_Sprites.emplace_back((textureRoot / R"(colored\dante\panelsinactive\panel1.png)").string().c_str()); //ColoredDantePanel1Inactive
        g_Sprites.emplace_back((textureRoot / R"(colored\dante\panelsinactive\panel2.png)").string().c_str()); //ColoredDantePanel2Inactive
        g_Sprites.emplace_back((textureRoot / R"(colored\dante\panelsinactive\panel3.png)").string().c_str()); //ColoredDantePanel3Inactive
        g_Sprites.emplace_back((textureRoot / R"(colored\dante\panelsinactive\panel4.png)").string().c_str()); //ColoredDantePanel4Inactive
        g_Sprites.emplace_back((textureRoot / R"(colored\dante\panelsinactive\panel5.png)").string().c_str()); //ColoredDantePanel5Inactive
    
        g_Sprites.emplace_back((textureRoot / R"(colored\dante\arrow1.png)").string().c_str()); //ColoredDanteArrow1
        g_Sprites.emplace_back((textureRoot / R"(colored\dante\arrow2.png)").string().c_str()); //ColoredDanteArrow2
        g_Sprites.emplace_back((textureRoot / R"(colored\dante\arrow3.png)").string().c_str()); //ColoredDanteArrow3
        g_Sprites.emplace_back((textureRoot / R"(colored\dante\arrow4.png)").string().c_str()); //ColoredDanteArrow4
        g_Sprites.emplace_back((textureRoot / R"(colored\dante\arrow5.png)").string().c_str()); //ColoredDanteArrow5
        g_Sprites.emplace_back((textureRoot / R"(colored\dante\center.png)").string().c_str()); //ColoredDanteCenter
    
        g_Sprites.emplace_back((textureRoot / R"(colored\vergil\panelsactive\panel1.png)").string().c_str()); //ColoredVergilPanel1Active
        g_Sprites.emplace_back((textureRoot / R"(colored\vergil\panelsactive\panel2.png)").string().c_str()); //ColoredVergilPanel2Active
        g_Sprites.emplace_back((textureRoot / R"(colored\vergil\panelsactive\panel3.png)").string().c_str()); //ColoredVergilPanel3Active
        g_Sprites.emplace_back((textureRoot / R"(colored\vergil\panelsactive\panel4.png)").string().c_str()); //ColoredVergilPanel4Active
        g_Sprites.emplace_back((textureRoot / R"(colored\vergil\panelsactive\panel5.png)").string().c_str()); //ColoredVergilPanel5Active
    
        g_Sprites.emplace_back((textureRoot / R"(colored\vergil\panelsinactive\panel1.png)").string().c_str()); //ColoredVergilPanel1Inactive
        g_Sprites.emplace_back((textureRoot / R"(colored\vergil\panelsinactive\panel2.png)").string().c_str()); //ColoredVergilPanel2Inactive
        g_Sprites.emplace_back((textureRoot / R"(colored\vergil\panelsinactive\panel3.png)").string().c_str()); //ColoredVergilPanel3Inactive
        g_Sprites.emplace_back((textureRoot / R"(colored\vergil\panelsinactive\panel4.png)").string().c_str()); //ColoredVergilPanel4Inactive
        g_Sprites.emplace_back((textureRoot / R"(colored\vergil\panelsinactive\panel5.png)").string().c_str()); //ColoredVergilPanel5Inactive
    
        g_Sprites.emplace_back((textureRoot / R"(colored\vergil\arrow1.png)").string().c_str()); //ColoredVergilArrow1
        g_Sprites.emplace_back((textureRoot / R"(colored\vergil\arrow2.png)").string().c_str()); //ColoredVergilArrow2
        g_Sprites.emplace_back((textureRoot / R"(colored\vergil\arrow3.png)").string().c_str()); //ColoredVergilArrow3
        g_Sprites.emplace_back((textureRoot / R"(colored\vergil\arrow4.png)").string().c_str()); //ColoredVergilArrow4
        g_Sprites.emplace_back((textureRoot / R"(colored\vergil\arrow5.png)").string().c_str()); //ColoredVergilArrow5
        g_Sprites.emplace_back((textureRoot / R"(colored\vergil\center.png)").string().c_str()); //ColoredVergilCenter
    
        g_Sprites.emplace_back((textureRoot / R"(colorless\panelsactive\panel1.png)").string().c_str()); //ColorlessPanel1Active
        g_Sprites.emplace_back((textureRoot / R"(colorless\panelsactive\panel2.png)").string().c_str()); //ColorlessPanel2Active
        g_Sprites.emplace_back((textureRoot / R"(colorless\panelsactive\panel3.png)").string().c_str()); //ColorlessPanel3Active
        g_Sprites.emplace_back((textureRoot / R"(colorless\panelsactive\panel4.png)").string().c_str()); //ColorlessPanel4Active
        g_Sprites.emplace_back((textureRoot / R"(colorless\panelsactive\panel5.png)").string().c_str()); //ColorlessPanel5Active
    
        g_Sprites.emplace_back((textureRoot / R"(colorless\panelsinactive\panel1.png)").string().c_str()); //ColorlessPanel1Inactive
        g_Sprites.emplace_back((textureRoot / R"(colorless\panelsinactive\panel2.png)").string().c_str()); //ColorlessPanel2Inactive
        g_Sprites.emplace_back((textureRoot / R"(colorless\panelsinactive\panel3.png)").string().c_str()); //ColorlessPanel3Inactive
        g_Sprites.emplace_back((textureRoot / R"(colorless\panelsinactive\panel4.png)").string().c_str()); //ColorlessPanel4Inactive
        g_Sprites.emplace_back((textureRoot / R"(colorless\panelsinactive\panel5.png)").string().c_str()); //ColorlessPanel5Inactive
    
        g_Sprites.emplace_back((textureRoot / R"(colorless\arrow1.png)").string().c_str()); //ColorlessArrow1
        g_Sprites.emplace_back((textureRoot / R"(colorless\arrow2.png)").string().c_str()); //ColorlessArrow2
        g_Sprites.emplace_back((textureRoot / R"(colorless\arrow3.png)").string().c_str()); //ColorlessArrow3
        g_Sprites.emplace_back((textureRoot / R"(colorless\arrow4.png)").string().c_str()); //ColorlessArrow4
        g_Sprites.emplace_back((textureRoot / R"(colorless\arrow5.png)").string().c_str()); //ColorlessArrow5
        g_Sprites.emplace_back((textureRoot / R"(colorless\center.png)").string().c_str()); //ColorlessCenter
    
        g_Sprites.emplace_back((textureRoot / R"(melee\active\rebellion-awakened.png)").string().c_str()); //RebellionAwakenedActive
        g_Sprites.emplace_back((textureRoot / R"(melee\active\rebellion-dormant.png)").string().c_str()); //RebellionDormantActive
        g_Sprites.emplace_back((textureRoot / R"(melee\active\cerberus.png)").string().c_str()); //CerberusActive
        g_Sprites.emplace_back((textureRoot / R"(melee\active\agnirudra.png)").string().c_str()); //AgniRudraActive
        g_Sprites.emplace_back((textureRoot / R"(melee\active\nevan.png)").string().c_str()); //NevanActive
        g_Sprites.emplace_back((textureRoot / R"(melee\active\beowulf.png)").string().c_str()); //BeowulfActive
        g_Sprites.emplace_back((textureRoot / R"(melee\active\forceedge-dante.png)").string().c_str()); //ForceEdgeActive
    
        g_Sprites.emplace_back((textureRoot / R"(guns\active\ebonyivory.png)").string().c_str()); //EbonyIvoryActive
        g_Sprites.emplace_back((textureRoot / R"(guns\active\shotgun.png)").string().c_str()); //ShotgunActive
        g_Sprites.emplace_back((textureRoot / R"(guns\active\artemis.png)").string().c_str()); //ArtemisActive
        g_Sprites.emplace_back((textureRoot / R"(guns\active\spiral.png)").string().c_str()); //SpiralActive
        g_Sprites.emplace_back((textureRoot / R"(guns\active\kalina.png)").string().c_str()); //KalinaActive
    
        g_Sprites.emplace_back((textureRoot / R"(melee\active\yamato.png)").string().c_str()); //YamatoActive
        g_Sprites.emplace_back((textureRoot / R"(melee\active\forceedge-vergil.png)").string().c_str()); //YamatoForceEdgeActive
    
        g_Sprites.emplace_back((textureRoot / R"(melee\active\rebellion-awakened.png)").string().c_str()); //DuplicateRebellionAwakenedActive
        g_Sprites.emplace_back((textureRoot / R"(melee\active\rebellion-dormant.png)").string().c_str()); //DuplicateRebellionDormantActive
        g_Sprites.emplace_back((textureRoot / R"(melee\active\cerberus.png)").string().c_str()); //DuplicateCerberusActive
        g_Sprites.emplace_back((textureRoot / R"(melee\active\agnirudra.png)").string().c_str()); //DuplicateAgniRudraActive
        g_Sprites.emplace_back((textureRoot / R"(melee\active\nevan.png)").string().c_str()); //DuplicateNevanActive
        g_Sprites.emplace_back((textureRoot / R"(melee\active\beowulf.png)").string().c_str()); //DuplicateBeowulfActive
        g_Sprites.emplace_back((textureRoot / R"(melee\active\forceedge-dante.png)").string().c_str()); //DuplicateForceEdgeActive
    
        g_Sprites.emplace_back((textureRoot / R"(guns\active\ebonyivory.png)").string().c_str()); //DuplicateEbonyIvoryActive
        g_Sprites.emplace_back((textureRoot / R"(guns\active\shotgun.png)").string().c_str()); //DuplicateShotgunActive
        g_Sprites.emplace_back((textureRoot / R"(guns\active\artemis.png)").string().c_str()); //DuplicateArtemisActive
        g_Sprites.emplace_back((textureRoot / R"(guns\active\spiral.png)").string().c_str()); //DuplicateSpiralActive
        g_Sprites.emplace_back((textureRoot / R"(guns\active\kalina.png)").string().c_str()); //DuplicateKalinaActive
    
        g_Sprites.emplace_back((textureRoot / R"(melee\active\yamato.png)").string().c_str()); //DuplicateYamatoActive
        g_Sprites.emplace_back((textureRoot / R"(melee\active\forceedge-vergil.png)").string().c_str()); //DuplicateYamatoForceEdgeActive
    
        g_Sprites.emplace_back((textureRoot / R"(melee\inactive\rebellion-awakened.png)").string().c_str()); //RebellionAwakenedInactive
        g_Sprites.emplace_back((textureRoot / R"(melee\inactive\rebellion-dormant.png)").string().c_str()); //RebellionDormantInactive
        g_Sprites.emplace_back((textureRoot / R"(melee\inactive\cerberus.png)").string().c_str()); //CerberusInactive
        g_Sprites.emplace_back((textureRoot / R"(melee\inactive\agnirudra.png)").string().c_str()); //AgniRudraInactive
        g_Sprites.emplace_back((textureRoot / R"(melee\inactive\nevan.png)").string().c_str()); //NevanInactive
        g_Sprites.emplace_back((textureRoot / R"(melee\inactive\beowulf.png)").string().c_str()); //BeowulfInactive
        g_Sprites.emplace_back((textureRoot / R"(melee\inactive\forceedge-dante.png)").string().c_str()); //ForceEdgeInactive
    
        g_Sprites.emplace_back((textureRoot / R"(guns\inactive\ebonyivory.png)").string().c_str()); //EbonyIvoryInactive
        g_Sprites.emplace_back((textureRoot / R"(guns\inactive\shotgun.png)").string().c_str()); //ShotgunInactive
        g_Sprites.emplace_back((textureRoot / R"(guns\inactive\artemis.png)").string().c_str()); //ArtemisInactive
        g_Sprites.emplace_back((textureRoot / R"(guns\inactive\spiral.png)").string().c_str()); //SpiralInactive
        g_Sprites.emplace_back((textureRoot / R"(guns\inactive\kalina.png)").string().c_str()); //KalinaInactive
    
        g_Sprites.emplace_back((textureRoot / R"(melee\inactive\yamato.png)").string().c_str()); //YamatoInactive
        g_Sprites.emplace_back((textureRoot / R"(melee\inactive\forceedge-vergil.png)").string().c_str()); //YamatoForceEdgeInactive
    }
    
    constexpr TextureID GetWeaponTextureID(WeaponIDs id, bool activeState)
    {
        switch (id)
        {
        case WeaponIDs::RebellionDormant:
            return activeState ? TextureID::RebellionDormantActive : TextureID::RebellionDormantInactive;
    
        case WeaponIDs::RebellionAwakened:
            return activeState ? TextureID::RebellionAwakenedActive : TextureID::RebellionAwakenedInactive;
    
        case WeaponIDs::ForceEdge:
            return activeState ? TextureID::ForceEdgeActive : TextureID::ForceEdgeInactive;
    
        case WeaponIDs::Cerberus:
            return activeState ? TextureID::CerberusActive : TextureID::CerberusInactive;
    
        case WeaponIDs::AgniRudra:
            return activeState ? TextureID::AgniRudraActive : TextureID::AgniRudraInactive;
    
        case WeaponIDs::Nevan:
            return activeState ? TextureID::NevanActive : TextureID::NevanInactive;
    
        case WeaponIDs::Beowulf:
            return activeState ? TextureID::BeowulfActive : TextureID::BeowulfInactive;
    
        case WeaponIDs::Yamato:
            return activeState ? TextureID::YamatoActive : TextureID::YamatoInactive;
    
        case WeaponIDs::ForceEdgeYamato:
            return activeState ? TextureID::YamatoForceEdgeActive : TextureID::YamatoForceEdgeInactive;
    
        case WeaponIDs::EbonyIvory:
            return activeState ? TextureID::EbonyIvoryActive : TextureID::EbonyIvoryInactive;
    
        case WeaponIDs::Shotgun:
            return activeState ? TextureID::ShotgunActive : TextureID::ShotgunInactive;
    
        case WeaponIDs::Artemis:
            return activeState ? TextureID::ArtemisActive : TextureID::ArtemisInactive;
    
        case WeaponIDs::Spiral:
            return activeState ? TextureID::SpiralActive : TextureID::SpiralInactive;
    
        case WeaponIDs::KalinaAnn:
            return activeState ? TextureID::KalinaActive : TextureID::KalinaInactive;
    
        default:
            return TextureID::Size;
        }
    
        return TextureID::Size;
    }
    
    constexpr TextureID GetDupAnimationWeaponTextureID(WeaponIDs id)
    {
        switch (id)
        {
        case WeaponIDs::RebellionDormant:
            return TextureID::DuplicateRebellionDormantActive;
    
        case WeaponIDs::RebellionAwakened:
            return TextureID::DuplicateRebellionAwakenedActive;
    
        case WeaponIDs::ForceEdge:
            return TextureID::DuplicateForceEdgeActive;
    
        case WeaponIDs::Cerberus:
            return TextureID::DuplicateCerberusActive;
    
        case WeaponIDs::AgniRudra:
            return TextureID::DuplicateAgniRudraActive;
    
        case WeaponIDs::Nevan:
            return TextureID::DuplicateNevanActive;
    
        case WeaponIDs::Beowulf:
            return TextureID::DuplicateBeowulfActive;
    
        case WeaponIDs::Yamato:
            return TextureID::DuplicateYamatoActive;
    
        case WeaponIDs::ForceEdgeYamato:
            return TextureID::DuplicateYamatoForceEdgeActive;
    
        case WeaponIDs::EbonyIvory:
            return TextureID::DuplicateEbonyIvoryActive;
    
        case WeaponIDs::Shotgun:
            return TextureID::DuplicateShotgunActive;
    
        case WeaponIDs::Artemis:
            return TextureID::DuplicateArtemisActive;
    
        case WeaponIDs::Spiral:
            return TextureID::DuplicateSpiralActive;
    
        case WeaponIDs::KalinaAnn:
            return TextureID::DuplicateKalinaActive;
    
        default:
            return TextureID::Size;
        }
    
        return TextureID::Size;
    }
    
    constexpr TextureID GetNeutralPanelTextureID(size_t slot, bool activeState)
    {
        switch (slot)
        {
        case 0:
            return activeState ? TextureID::ColorlessPanel1Active : TextureID::ColorlessPanel1Inactive;
    
        case 1:
            return activeState ? TextureID::ColorlessPanel2Active : TextureID::ColorlessPanel2Inactive;
    
        case 2:
            return activeState ? TextureID::ColorlessPanel3Active : TextureID::ColorlessPanel3Inactive;
    
        case 3:
            return activeState ? TextureID::ColorlessPanel4Active : TextureID::ColorlessPanel4Inactive;
    
        case 4:
            return activeState ? TextureID::ColorlessPanel5Active : TextureID::ColorlessPanel5Inactive;
    
        default:
            return TextureID::Size;
        }
    
        return TextureID::Size;
    }
    
    constexpr TextureID GetDantePanelTextureID(size_t slot, bool activeState)
    {
        switch (slot)
        {
        case 0:
            return activeState ? TextureID::ColoredDantePanel1Active : TextureID::ColoredDantePanel1Inactive;
    
        case 1:
            return activeState ? TextureID::ColoredDantePanel2Active : TextureID::ColoredDantePanel2Inactive;
    
        case 2:
            return activeState ? TextureID::ColoredDantePanel3Active : TextureID::ColoredDantePanel3Inactive;
    
        case 3:
            return activeState ? TextureID::ColoredDantePanel4Active : TextureID::ColoredDantePanel4Inactive;
    
        case 4:
            return activeState ? TextureID::ColoredDantePanel5Active : TextureID::ColoredDantePanel5Inactive;
    
        default:
            return TextureID::Size;
        }
    
        return TextureID::Size;
    }
    
    constexpr TextureID GetVergilPanelTextureID(size_t slot, bool activeState)
    {
        switch (slot)
        {
        case 0:
            return activeState ? TextureID::ColoredVergilPanel1Active : TextureID::ColoredVergilPanel1Inactive;
    
        case 1:
            return activeState ? TextureID::ColoredVergilPanel2Active : TextureID::ColoredVergilPanel2Inactive;
    
        case 2:
            return activeState ? TextureID::ColoredVergilPanel3Active : TextureID::ColoredVergilPanel3Inactive;
    
        case 3:
            return activeState ? TextureID::ColoredVergilPanel4Active : TextureID::ColoredVergilPanel4Inactive;
    
        case 4:
            return activeState ? TextureID::ColoredVergilPanel5Active : TextureID::ColoredVergilPanel5Inactive;
    
        default:
            return TextureID::Size;
        }
    
        return TextureID::Size;
    }
    
    
    constexpr TextureID GetNeutralArrowTextureID(size_t slot)
    {
        switch (slot)
        {
        case 0:
            return TextureID::ColorlessArrow1;
    
        case 1:
            return TextureID::ColorlessArrow2;
    
        case 2:
            return TextureID::ColorlessArrow3;
    
        case 3:
            return TextureID::ColorlessArrow4;
    
        case 4:
            return TextureID::ColorlessArrow5;
    
        default:
            return TextureID::Size;
        }
    
        return TextureID::Size;
    }
    
    constexpr TextureID GetDanteArrowTextureID(size_t slot)
    {
        switch (slot)
        {
        case 0:
            return TextureID::ColoredDanteArrow1;
    
        case 1:
            return TextureID::ColoredDanteArrow2;
    
        case 2:
            return TextureID::ColoredDanteArrow3;
    
        case 3:
            return TextureID::ColoredDanteArrow4;
    
        case 4:
            return TextureID::ColoredDanteArrow5;
    
        default:
            return TextureID::Size;
        }
    
        return TextureID::Size;
    }
    
    constexpr TextureID GetVergilArrowTextureID(size_t slot)
    {
        switch (slot)
        {
        case 0:
            return TextureID::ColoredVergilArrow1;
    
        case 1:
            return TextureID::ColoredVergilArrow2;
    
        case 2:
            return TextureID::ColoredVergilArrow3;
    
        case 3:
            return TextureID::ColoredVergilArrow4;
    
        case 4:
            return TextureID::ColoredVergilArrow5;
    
        default:
            return TextureID::Size;
        }
    
        return TextureID::Size;
    }
    
    
    constexpr TextureID GetPanelTextureID(WheelThemes theme, size_t panelSlot, bool activeState)
    {
        switch (theme)
        {
        case WheelThemes::Neutral:
            return GetNeutralPanelTextureID(panelSlot, activeState);
            
        case WheelThemes::Dante:
            return GetDantePanelTextureID(panelSlot, activeState);
    
        case WheelThemes::Vergil:
            return GetVergilPanelTextureID(panelSlot, activeState);
    
        default:
            return TextureID::Size;
        }
    
        return TextureID::Size;
    }
    
    constexpr TextureID GetArrowTextureID(WheelThemes theme, size_t panelSlot)
    {
        switch (theme)
        {
        case WheelThemes::Neutral:
            return GetNeutralArrowTextureID(panelSlot);
    
        case WheelThemes::Dante:
            return GetDanteArrowTextureID(panelSlot);
    
        case WheelThemes::Vergil:
            return GetVergilArrowTextureID(panelSlot);
    
        default:
            return TextureID::Size;
        }
    
        return TextureID::Size;
    }
    
    constexpr TextureID GetCenterTextureID(WheelThemes theme)
    {
        switch (theme)
        {
        case WheelThemes::Neutral:
            return TextureID::ColorlessCenter;
    
        case WheelThemes::Dante:
            return TextureID::ColoredDanteCenter;
    
        case WheelThemes::Vergil:
            return TextureID::ColoredVergilCenter;
    
        default:
            return TextureID::Size;
        }
    
        return TextureID::Size;
    }
    
    constexpr std::vector<size_t> GetSpriteIDs(WheelThemes themeID, const std::vector<WeaponIDs>& weaponIDs)
    {
        std::vector<size_t> spriteIds;
    
        spriteIds.reserve(weaponIDs.size() * 6);
    
        // Order matters here, back to front
        {
            spriteIds.push_back((size_t)GetCenterTextureID(themeID)); // 1st: The center piece
    
            for (size_t i = 0; i < weaponIDs.size(); i++)
            {
                if (i > 4)
                    break;
    
                spriteIds.push_back((size_t)GetArrowTextureID(themeID, i)); // 2nd: The arrows
            }
    
            for (size_t i = 0; i < weaponIDs.size(); i++)
            {
                if (i > 4)
                    break;
    
                spriteIds.push_back((size_t)GetPanelTextureID(themeID, i, false)); // 3rd: The inactive panels
            }
    
            for (size_t i = 0; i < weaponIDs.size(); i++)
            {
                if (i > 4)
                    break;
    
                spriteIds.push_back((size_t)GetPanelTextureID(themeID, i, false)); // 3rd: The inactive panels
            }
    
            for (size_t i = 0; i < weaponIDs.size(); i++)
            {
                if (i > 4)
                    break;
    
                spriteIds.push_back((size_t)GetPanelTextureID(themeID, i, true)); // 4th: The active panels
            }
    
            for (size_t i = 0; i < weaponIDs.size(); i++)
            {
                if (i > 4)
                    break;
    
                spriteIds.push_back((size_t)GetWeaponTextureID(weaponIDs[i], false)); // 5th: The inactive weapons
            }
    
            for (size_t i = 0; i < weaponIDs.size(); i++)
            {
                if (i > 4)
                    break;
    
                spriteIds.push_back((size_t)GetWeaponTextureID(weaponIDs[i], true)); // 6th: The active weapons
            }
    
            for (size_t i = 0; i < weaponIDs.size(); i++)
            {
                if (i > 4)
                    break;
    
                spriteIds.push_back((size_t)GetDupAnimationWeaponTextureID(weaponIDs[i])); // 7th: The active weapon animation duplicates
            }
        }
    
        return spriteIds;
    }

    WeaponWheel::WeaponWheel(ID3D11Device* pD3D11Device, ID3D11DeviceContext* pD3D11DeviceContext, UINT width, UINT height,
        std::vector<WeaponIDs> weapons, WheelThemes themeID)
        : m_pD3D11Device(pD3D11Device), m_pD3D11DeviceContext(pD3D11DeviceContext), m_Width(width), m_Height(height),
        m_Weapons(weapons), m_ThemeID(themeID)
    {
        if (g_Sprites.size() == 0)
            LoadSprites();

        const auto spriteIDs = GetSpriteIDs(m_ThemeID, m_Weapons);

        m_pSpriteBatch = std::make_unique<Graphics::BatchedSprites>(m_pD3D11Device, m_Width, m_Height, g_Sprites, spriteIDs);

        for (size_t i = 0; i < m_Weapons.size(); i++)
        {
			// Adjust SetTransform to include slot index in the ID
			m_pSpriteBatch->SetTransform(
				(size_t)GetWeaponTextureID(m_Weapons[i], false) + i,  // Unique key per slot
				s_MeleeInactiveSlotTransforms[i].Translation,
				s_MeleeInactiveSlotTransforms[i].Rotation,
				s_MeleeInactiveSlotTransforms[i].Scale
			);

			m_pSpriteBatch->SetTransform(
				(size_t)GetWeaponTextureID(m_Weapons[i], true) + i,   // Unique key per slot
				s_MeleeActiveSlotTransforms[i].Translation,
				s_MeleeActiveSlotTransforms[i].Rotation,
				s_MeleeActiveSlotTransforms[i].Scale
			);

			m_pSpriteBatch->SetTransform(
				(size_t)GetDupAnimationWeaponTextureID(m_Weapons[i]) + i,  // Unique key per slot
				s_MeleeActiveSlotTransforms[i].Translation,
				s_MeleeActiveSlotTransforms[i].Rotation,
				s_MeleeActiveSlotTransforms[i].Scale
			);

			// Opacity also should be unique per slot
			m_pSpriteBatch->SetOpacity(
				(size_t)GetDupAnimationWeaponTextureID(m_Weapons[i]) + i,  // Unique key per slot
				0.0f
			);
        }

        InitializeAnimations();
    }

    WeaponWheel::~WeaponWheel()
    {}

    void WeaponWheel::SetWeapons(std::vector<WeaponIDs> weapons)
    {
        m_Weapons = weapons;

        const auto spriteIDs = GetSpriteIDs(m_ThemeID, m_Weapons);

        m_pSpriteBatch->SetActiveSprites(spriteIDs);

        UpdateSlotStates();
    }

    void WeaponWheel::SetWheelTheme(WheelThemes themeID)
    {
        m_ThemeID = themeID;

        const auto spriteIDs = GetSpriteIDs(m_ThemeID, m_Weapons);

        m_pSpriteBatch->SetActiveSprites(spriteIDs);

        UpdateSlotStates();
    }

    void WeaponWheel::SetActiveSlot(size_t slot)
    {
        m_pWheelFadeAnimation->OnReset();
        m_pActiveWeaponFadeAnimation->OnReset();
        m_pWeaponSwitchScaleAnimation->OnReset();
        m_pWeaponSwitchBrightnessAnimation->OnReset();

        m_CurrentActiveSlot = slot;

        UpdateSlotStates();

        m_AlreadyTriggeredWheelFadeAnim = false;
        m_AlreadyTriggeredActiveWeaponFadeAnim = false;
        m_AlreadyTriggeredSwitchBrightnessAnim = false;
        m_AlreadyTriggeredSwitchScaleAnim = false;
        m_SinceLatestChangeMs = 0.0f;
        m_SinceLatestChangeMsGlobal = 0.0f;
    }

    void WeaponWheel::OnUpdate(double ts, double tsGlobal)
    {
        if (!m_AlreadyTriggeredWheelFadeAnim && m_SinceLatestChangeMsGlobal >= s_FadeDelay)
        {
            m_RunWheelFadeAnim = true;
            m_AlreadyTriggeredWheelFadeAnim = true;
        }

        if (!m_AlreadyTriggeredActiveWeaponFadeAnim && m_SinceLatestChangeMsGlobal >= s_FadeDelay)
        {
            m_RunActiveWeaponFadeAnim = true;
            m_AlreadyTriggeredActiveWeaponFadeAnim = true;
        }

        if (!m_AlreadyTriggeredSwitchScaleAnim && m_SinceLatestChangeMs >= 5)
        {
            m_RunWeaponSwitchScaleAnim = true;
            m_AlreadyTriggeredSwitchScaleAnim = true;
        }

        if (!m_AlreadyTriggeredSwitchBrightnessAnim && m_SinceLatestChangeMs >= 5)
        {
            m_RunWeaponSwitchBrightnessAnim = true;
            m_AlreadyTriggeredSwitchBrightnessAnim = true;
        }

        if (m_RunWheelFadeAnim)
            m_pWheelFadeAnimation->OnUpdate(ts);

        if (m_RunActiveWeaponFadeAnim)
            m_pActiveWeaponFadeAnimation->OnUpdate(ts);

        if (m_RunWeaponSwitchScaleAnim)
            m_pWeaponSwitchScaleAnimation->OnUpdate(ts);

        if (m_RunWeaponSwitchBrightnessAnim)
            m_pWeaponSwitchBrightnessAnimation->OnUpdate(ts);

        m_SinceLatestChangeMs += ts;
        m_SinceLatestChangeMsGlobal += tsGlobal;
    }

    bool WeaponWheel::OnDraw()
    {
        return m_pSpriteBatch->Draw(m_pD3D11DeviceContext);
    }

    ID3D11ShaderResourceView* WeaponWheel::GetSRV()
    {
        return m_pSpriteBatch->GetRTSRV();
    }

    const std::string& WeaponWheel::GetLastRenderingErrorMsg()
    {
        return m_pSpriteBatch->GetLastErrorMsg();
    }

    HRESULT WeaponWheel::GetLastRenderingErrorCode()
    {
        return m_pSpriteBatch->GetLastErrorCode();
    }

    void WeaponWheel::InitializeAnimations()
    {
        {
            m_pWheelFadeAnimation = std::make_unique<GenericAnimation>(s_WheelFadeoutDur);

            // Before the animation starts
            m_pWheelFadeAnimation->SetOnStart([this](GenericAnimation* pAnim)
                {
                    // Ensure the state is set to normal
                    UpdateSlotStates();
                });

            // On update
            m_pWheelFadeAnimation->SetOnUpdate([this](GenericAnimation* pAnim)
                {
                    const auto progress = pAnim->GetProgressNormalized();

                    for (size_t i = 0; i < m_Weapons.size(); i++)
                    {
                        // Inactive slots opacity
                        if (i != m_CurrentActiveSlot)
                        {
                            m_pSpriteBatch->SetOpacity((size_t)GetWeaponTextureID(m_Weapons[i], false), 1.0f - progress);
                            m_pSpriteBatch->SetOpacity((size_t)GetPanelTextureID(m_ThemeID, i, false), 1.0f - progress);
                        }
                        else // Active slots opacity
                        {
                            m_pSpriteBatch->SetOpacity((size_t)GetArrowTextureID(m_ThemeID, i), 1.0f - progress);
                            m_pSpriteBatch->SetOpacity((size_t)GetPanelTextureID(m_ThemeID, i, true), 1.0f - progress);
                        }
                    }

                    m_pSpriteBatch->SetOpacity((size_t)GetCenterTextureID(m_ThemeID), 1.0f - progress);
                });

            // After the animation ends
            m_pWheelFadeAnimation->SetOnEnd([this](GenericAnimation* pAnim)
                {
                    m_RunWheelFadeAnim = false;
                });

            // When reset
            m_pWheelFadeAnimation->SetOnReset([this](GenericAnimation* pAnim)
                {
                    m_RunWheelFadeAnim = false;
                    UpdateSlotStates();
                });
        }

        {
            m_pActiveWeaponFadeAnimation = std::make_unique<GenericAnimation>(s_ActiveWeaponFadeoutDur);

            // Before the animation starts
            m_pActiveWeaponFadeAnimation->SetOnStart([this](GenericAnimation* pAnim)
                {
                    // Ensure the active weapon is at full opacity
                    m_pSpriteBatch->SetOpacity((size_t)GetWeaponTextureID(m_Weapons[m_CurrentActiveSlot], true), 1.0f);
                });

            // On update
            m_pActiveWeaponFadeAnimation->SetOnUpdate([this](GenericAnimation* pAnim)
                {
                    const auto progress = pAnim->GetProgressNormalized();

                    m_pSpriteBatch->SetOpacity((size_t)GetWeaponTextureID(m_Weapons[m_CurrentActiveSlot], true), 1.0f - progress);
                });

            // After the animation ends
            m_pActiveWeaponFadeAnimation->SetOnEnd([this](GenericAnimation* pAnim)
                {
                    m_RunActiveWeaponFadeAnim = false;
                });

            // When reset
            m_pActiveWeaponFadeAnimation->SetOnReset([this](GenericAnimation* pAnim)
                {
                    m_RunActiveWeaponFadeAnim = false;
                    UpdateSlotStates();
                });
        }

        {
            m_pWeaponSwitchBrightnessAnimation = std::make_unique<GenericAnimation>(s_SwitchBrightnessAnimDur);

            // Before the animation starts
            m_pWeaponSwitchBrightnessAnimation->SetOnStart([this](GenericAnimation* pAnim)
                {
                    // Ensure the active weapon is at normal brightness
                    m_pSpriteBatch->SetBrightness((size_t)GetWeaponTextureID(m_Weapons[m_CurrentActiveSlot], true), 1.0f);
                });

            // On update
            m_pWeaponSwitchBrightnessAnimation->SetOnUpdate([this](GenericAnimation* pAnim)
                {
                    const auto progress = pAnim->GetProgressNormalized();

                    m_pSpriteBatch->SetBrightness((size_t)GetWeaponTextureID(m_Weapons[m_CurrentActiveSlot], true), progress * 3.18f);
                });

            // After the animation ends
            m_pWeaponSwitchBrightnessAnimation->SetOnEnd([this](GenericAnimation* pAnim)
                {
                    m_RunWeaponSwitchBrightnessAnim = false;
                    m_pSpriteBatch->SetBrightness((size_t)GetWeaponTextureID(m_Weapons[m_CurrentActiveSlot], true), 1.0f);
                });

            // When reset
            m_pWeaponSwitchBrightnessAnimation->SetOnReset([this](GenericAnimation* pAnim)
                {
                    m_RunWeaponSwitchBrightnessAnim = false;
                    m_pSpriteBatch->SetBrightness((size_t)GetWeaponTextureID(m_Weapons[m_CurrentActiveSlot], true), 1.0f);
                });
        }

        {
            m_pWeaponSwitchScaleAnimation = std::make_unique<GenericAnimation>(s_SwitchScaleAnimDur);

            // Before the animation starts
            m_pWeaponSwitchScaleAnimation->SetOnStart([this](GenericAnimation* pAnim)
                {
                    m_pSpriteBatch->SetOpacity(
                        (size_t)GetDupAnimationWeaponTextureID(m_Weapons[m_CurrentActiveSlot]), 0.0f);

                    m_pSpriteBatch->SetBrightness(
                        (size_t)GetDupAnimationWeaponTextureID(m_Weapons[m_CurrentActiveSlot]), 1.0f);

                    m_pSpriteBatch->ScaleTo(
                        (size_t)GetDupAnimationWeaponTextureID(m_Weapons[m_CurrentActiveSlot]), glm::vec3(1.0f));
                });

            // On update
            m_pWeaponSwitchScaleAnimation->SetOnUpdate([this](GenericAnimation* pAnim)
                {
                    const auto progress = pAnim->GetProgressNormalized();

                    m_pSpriteBatch->SetOpacity(
                        (size_t)GetDupAnimationWeaponTextureID(m_Weapons[m_CurrentActiveSlot]), progress * 0.9f);

                    m_pSpriteBatch->SetBrightness(
                        (size_t)GetDupAnimationWeaponTextureID(m_Weapons[m_CurrentActiveSlot]), progress * 3.18f);

                    m_pSpriteBatch->ScaleTo(
                        (size_t)GetDupAnimationWeaponTextureID(m_Weapons[m_CurrentActiveSlot]),
                        s_MeleeActiveSlotTransforms[m_CurrentActiveSlot].Scale * float(progress) * 1.5f);
                });

            // After the animation ends
            m_pWeaponSwitchScaleAnimation->SetOnEnd([this](GenericAnimation* pAnim)
                {
                    m_RunWeaponSwitchScaleAnim = false;

                    m_pSpriteBatch->SetOpacity(
                        (size_t)GetDupAnimationWeaponTextureID(m_Weapons[m_CurrentActiveSlot]), 0.0f);

                    m_pSpriteBatch->SetBrightness(
                        (size_t)GetDupAnimationWeaponTextureID(m_Weapons[m_CurrentActiveSlot]), 1.0f);

                    m_pSpriteBatch->ScaleTo(
                        (size_t)GetDupAnimationWeaponTextureID(m_Weapons[m_CurrentActiveSlot]), glm::vec3(1.0f));
                });

            // When reset
            m_pWeaponSwitchScaleAnimation->SetOnReset([this](GenericAnimation* pAnim)
                {
                    m_RunWeaponSwitchScaleAnim = false;

                    m_pSpriteBatch->SetOpacity(
                        (size_t)GetDupAnimationWeaponTextureID(m_Weapons[m_CurrentActiveSlot]), 0.0f);

                    m_pSpriteBatch->SetBrightness(
                        (size_t)GetDupAnimationWeaponTextureID(m_Weapons[m_CurrentActiveSlot]), 1.0f);

                    m_pSpriteBatch->ScaleTo(
                        (size_t)GetDupAnimationWeaponTextureID(m_Weapons[m_CurrentActiveSlot]), glm::vec3(1.0f));
                });
        }
    }

    void WeaponWheel::SetInactiveOpacity(float opacity)
    {
        for (const auto& weaponID : m_Weapons)
            m_pSpriteBatch->SetOpacity((size_t)GetWeaponTextureID(weaponID, false), opacity);
    }

    void WeaponWheel::SetActiveWeaponOpacity(float opacity)
    {
        if (m_CurrentActiveSlot > 4) // 5 slots is max
            return;

        m_pSpriteBatch->SetOpacity((size_t)GetWeaponTextureID(m_Weapons[m_CurrentActiveSlot], true), opacity);
    }

    void WeaponWheel::UpdateSlotStates()
    {
        if (m_CurrentActiveSlot > 4)
        {
            for (size_t i = 0; i < m_Weapons.size(); i++)
            {
                m_pSpriteBatch->SetOpacity((size_t)GetWeaponTextureID(m_Weapons[i], false), 1.0f);
                m_pSpriteBatch->SetOpacity((size_t)GetWeaponTextureID(m_Weapons[i], true), 0.0f);
                m_pSpriteBatch->SetOpacity((size_t)GetPanelTextureID(m_ThemeID, i, false), 1.0f);
                m_pSpriteBatch->SetOpacity((size_t)GetPanelTextureID(m_ThemeID, i, true), 0.0f);
                m_pSpriteBatch->SetOpacity((size_t)GetArrowTextureID(m_ThemeID, i), 1.0f);
            }

            m_pSpriteBatch->SetOpacity((size_t)GetCenterTextureID(m_ThemeID), 1.0f);

            return;
        }

        for (size_t i = 0; i < m_Weapons.size(); i++)
        {
            m_pSpriteBatch->SetOpacity((size_t)GetWeaponTextureID(m_Weapons[i], false), 1.0f);
            m_pSpriteBatch->SetOpacity((size_t)GetWeaponTextureID(m_Weapons[i], true), 0.0f);
            m_pSpriteBatch->SetOpacity((size_t)GetPanelTextureID(m_ThemeID, i, false), 1.0f);
            m_pSpriteBatch->SetOpacity((size_t)GetPanelTextureID(m_ThemeID, i, true), 0.0f);
            m_pSpriteBatch->SetOpacity((size_t)GetArrowTextureID(m_ThemeID, i), 0.0f);
        }

        m_pSpriteBatch->SetOpacity((size_t)GetCenterTextureID(m_ThemeID), 1.0f);

        m_pSpriteBatch->SetOpacity((size_t)GetWeaponTextureID(m_Weapons[m_CurrentActiveSlot], false), 0.0f);
        m_pSpriteBatch->SetOpacity((size_t)GetPanelTextureID(m_ThemeID, m_CurrentActiveSlot, false), 0.0f);
        m_pSpriteBatch->SetOpacity((size_t)GetPanelTextureID(m_ThemeID, m_CurrentActiveSlot, true), 1.0f);
        m_pSpriteBatch->SetOpacity((size_t)GetArrowTextureID(m_ThemeID, m_CurrentActiveSlot), 1.0f);

        m_pSpriteBatch->SetOpacity((size_t)GetWeaponTextureID(m_Weapons[m_CurrentActiveSlot], true), 1.0f);
    }
}