#include "WeaponWheel.hpp"

#include "Sprite.hpp"

#include <filesystem>

std::vector<Graphics::SpriteDesc> g_SpriteDescs;

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
        glm::vec3(0.416f, 0.287f, 0.0f),
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

static constexpr Transform s_RangedActiveSlotTransforms[5] =
{
    { // Slot 1
        glm::vec3(-0.278f, 0.486f, 0.0f),
        glm::vec3(0.0f, 0.0f, glm::radians(49.831f)),
        glm::vec3(0.446f)
    },
    { // Slot 2
        glm::vec3(0.522f, 0.293f, 0.0f),
        glm::vec3(0.0f, 0.0f, glm::radians(-11.186f)),
        glm::vec3(0.420f)
    },
    { // Slot 3
        glm::vec3(0.472f, -0.344f, 0.0f),
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

static constexpr Transform s_MeleeActiveSlotSwitchVersTransforms[5] =
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

static constexpr Transform s_MeleeActiveSlotTransforms[5] =
{
    { // Slot 1
        glm::vec3(-0.142f, 0.416f, 0.0f),
        glm::vec3(0.0f, 0.0f, glm::radians(49.831f)),
        glm::vec3(0.446f)
    },
    { // Slot 2
        glm::vec3(0.416f, 0.287f, 0.0f),
        glm::vec3(0.0f, 0.0f, glm::radians(-11.186f)),
        glm::vec3(0.420f)
    },
    { // Slot 3
        glm::vec3(0.384f, -0.330f, 0.0f),
        glm::vec3(0.0f, 0.0f, glm::radians(23.390f)),
        glm::vec3(0.388f)
    },
    { // Slot 4
        glm::vec3(-0.086f, -0.504f, 0.0f),
        glm::vec3(0.0f, 0.0f, glm::radians(78.486f)),
        glm::vec3(0.391f)
    },
    { // Slot 5
        glm::vec3(-0.524f, -0.016f, 0.0f),
        glm::vec3(0.0f, 0.0f, glm::radians(23.254f)),
        glm::vec3(0.359f)
    }
};

static constexpr glm::vec3 s_WeaponInactiveSwitchVersRotations[] {
    { // RebellionDormant
        glm::vec3(0.0f, 0.0f, glm::radians(55.120f))
    },
    { // RebellionAwakened
        glm::vec3(0.0f, 0.0f, glm::radians(55.120f))
    },
    { // ForceEdge
        glm::vec3(0.0f, 0.0f, glm::radians(55.120f))
    },
    { // Cerberus
        glm::vec3(0.0f, 0.0f, glm::radians(-12.919f))
    },
    { // AgniRudra
        glm::vec3(0.0f, 0.0f, glm::radians(27.560f))
    },
    { // Nevan
        glm::vec3(0.0f, 0.0f, glm::radians(80.366f))
    },
    { // Beowulf
        glm::vec3(0.0f, 0.0f, glm::radians(23.254f))
    },
    { // Yamato
        glm::vec3(0.0f, 0.0f, glm::radians(55.120f))
    },
    { // ForceEdgeYamato
        glm::vec3(0.0f, 0.0f, glm::radians(27.560f))
    },

    { // EbonyIvory
        glm::vec3(0.0f, 0.0f, glm::radians(0.0f))
    },
    { // Shotgun
        glm::vec3(0.0f, 0.0f, glm::radians(0.0f))
    },
    { // Artemis
        glm::vec3(0.0f, 0.0f, glm::radians(0.0f))
    },
    { // Spiral
        glm::vec3(0.0f, 0.0f, glm::radians(0.0f))
    },
    { // KalinaAnn
        glm::vec3(0.0f, 0.0f, glm::radians(0.0f))
    },
};

static constexpr glm::vec3 s_WeaponActiveSwitchVersRotations[] {
    { // RebellionDormant
        glm::vec3(0.0f, 0.0f, glm::radians(49.831f))
    },
    { // RebellionAwakened
        glm::vec3(0.0f, 0.0f, glm::radians(49.831f))
    },
    { // ForceEdge
        glm::vec3(0.0f, 0.0f, glm::radians(49.831f))
    },
    { // Cerberus
        glm::vec3(0.0f, 0.0f, glm::radians(-11.186f))
    },
    { // AgniRudra
        glm::vec3(0.0f, 0.0f, glm::radians(23.390f))
    },
    { // Nevan
        glm::vec3(0.0f, 0.0f, glm::radians(78.486f))
    },
    { // Beowulf
        glm::vec3(0.0f, 0.0f, glm::radians(23.254f))
    },
    { // Yamato
        glm::vec3(0.0f, 0.0f, glm::radians(49.831f))
    },
    { // ForceEdgeYamato
        glm::vec3(0.0f, 0.0f, glm::radians(23.390f))
    },

    { // EbonyIvory
        glm::vec3(0.0f, 0.0f, glm::radians(0.0f))
    },
    { // Shotgun
        glm::vec3(0.0f, 0.0f, glm::radians(0.0f))
    },
    { // Artemis
        glm::vec3(0.0f, 0.0f, glm::radians(0.0f))
    },
    { // Spiral
        glm::vec3(0.0f, 0.0f, glm::radians(0.0f))
    },
    { // KalinaAnn
        glm::vec3(0.0f, 0.0f, glm::radians(0.0f))
    },
};

static constexpr glm::vec3 s_WeaponNormalRotations[]{
    { // RebellionDormant
        glm::vec3(0.0f, 0.0f, glm::radians(-22.572f))
    },
    { // RebellionAwakened
        glm::vec3(0.0f, 0.0f, glm::radians(-22.572f))
    },
    { // ForceEdge
        glm::vec3(0.0f, 0.0f, glm::radians(-22.572f))
    },
    { // Cerberus
        glm::vec3(0.0f, 0.0f, glm::radians(0.0f))
    },
    { // AgniRudra
        glm::vec3(0.0f, 0.0f, glm::radians(0.0f))
    },
    { // Nevan
        glm::vec3(0.0f, 0.0f, glm::radians(0.0f))
    },
    { // Beowulf
        glm::vec3(0.0f, 0.0f, glm::radians(0.0f))
    },
    { // Yamato
        glm::vec3(0.0f, 0.0f, glm::radians(49.831f))
    },
    { // ForceEdgeYamato
        glm::vec3(0.0f, 0.0f, glm::radians(23.390f))
    },

    { // EbonyIvory
        glm::vec3(0.0f, 0.0f, glm::radians(0.0f))
    },
    { // Shotgun
        glm::vec3(0.0f, 0.0f, glm::radians(0.0f))
    },
    { // Artemis
        glm::vec3(0.0f, 0.0f, glm::radians(0.0f))
    },
    { // Spiral
        glm::vec3(0.0f, 0.0f, glm::radians(0.0f))
    },
    { // KalinaAnn
        glm::vec3(0.0f, 0.0f, glm::radians(0.0f))
    },
};

static constexpr glm::vec3 s_WeaponNormalInactiveScales[] {
    { // RebellionDormant
        glm::vec3(0.244f)
    },
    { // RebellionAwakened
        glm::vec3(0.244f)
    },
    { // ForceEdge
        glm::vec3(0.244f)
    },
    { // Cerberus
        glm::vec3(0.227f)
    },
    { // AgniRudra
        glm::vec3(0.178f)
    },
    { // Nevan
        glm::vec3(0.217f)
    },
    { // Beowulf
        glm::vec3(0.214f)
    },
    { // Yamato
        glm::vec3(0.244f)
    },
    { // ForceEdgeYamato
        glm::vec3(0.178f)
    },

    { // EbonyIvory
        glm::vec3(0.214f)
    },
    { // Shotgun
        glm::vec3(0.227f)
    },
    { // Artemis
        glm::vec3(0.227f)
    },
    { // Spiral
        glm::vec3(0.217f)
    },
    { // KalinaAnn
        glm::vec3(0.214f)
    },
};

static constexpr glm::vec3 s_WeaponNormalActiveScales[] {
    { // RebellionDormant
        glm::vec3(0.446f)
    },
    { // RebellionAwakened
        glm::vec3(0.446f)
    },
    { // ForceEdge
        glm::vec3(0.446f)
    },
    { // Cerberus
        glm::vec3(0.420f)
    },
    { // AgniRudra
        glm::vec3(0.388f)
    },
    { // Nevan
        glm::vec3(0.391f)
    },
    { // Beowulf
        glm::vec3(0.359f)
    },
    { // Yamato
        glm::vec3(0.446f)
    },
    { // ForceEdgeYamato
        glm::vec3(0.388f)
    },

    { // EbonyIvory
        glm::vec3(0.391f)
    },
    { // Shotgun
        glm::vec3(0.420f)
    },
    { // Artemis
        glm::vec3(0.420f)
    },
    { // Spiral
        glm::vec3(0.391f)
    },
    { // KalinaAnn
        glm::vec3(0.359f)
    },
};

namespace WW
{
    void LoadSpriteDescs()
    {
        g_SpriteDescs.reserve((size_t)TextureID::Size);

        const std::filesystem::path textureRoot(R"(Crimson\assets\weaponwheel)");

        g_SpriteDescs.emplace_back((textureRoot / R"(colored\dante\panelsactive\panel1.png)").string().c_str()); //ColoredDantePanel1Active
        g_SpriteDescs.emplace_back((textureRoot / R"(colored\dante\panelsactive\panel2.png)").string().c_str()); //ColoredDantePanel2Active
        g_SpriteDescs.emplace_back((textureRoot / R"(colored\dante\panelsactive\panel3.png)").string().c_str()); //ColoredDantePanel3Active
        g_SpriteDescs.emplace_back((textureRoot / R"(colored\dante\panelsactive\panel4.png)").string().c_str()); //ColoredDantePanel4Active
        g_SpriteDescs.emplace_back((textureRoot / R"(colored\dante\panelsactive\panel5.png)").string().c_str()); //ColoredDantePanel5Active

        g_SpriteDescs.emplace_back((textureRoot / R"(colored\dante\panelsinactive\panel1.png)").string().c_str()); //ColoredDantePanel1Inactive
        g_SpriteDescs.emplace_back((textureRoot / R"(colored\dante\panelsinactive\panel2.png)").string().c_str()); //ColoredDantePanel2Inactive
        g_SpriteDescs.emplace_back((textureRoot / R"(colored\dante\panelsinactive\panel3.png)").string().c_str()); //ColoredDantePanel3Inactive
        g_SpriteDescs.emplace_back((textureRoot / R"(colored\dante\panelsinactive\panel4.png)").string().c_str()); //ColoredDantePanel4Inactive
        g_SpriteDescs.emplace_back((textureRoot / R"(colored\dante\panelsinactive\panel5.png)").string().c_str()); //ColoredDantePanel5Inactive

        g_SpriteDescs.emplace_back((textureRoot / R"(colored\dante\arrow1.png)").string().c_str()); //ColoredDanteArrow1
        g_SpriteDescs.emplace_back((textureRoot / R"(colored\dante\arrow2.png)").string().c_str()); //ColoredDanteArrow2
        g_SpriteDescs.emplace_back((textureRoot / R"(colored\dante\arrow3.png)").string().c_str()); //ColoredDanteArrow3
        g_SpriteDescs.emplace_back((textureRoot / R"(colored\dante\arrow4.png)").string().c_str()); //ColoredDanteArrow4
        g_SpriteDescs.emplace_back((textureRoot / R"(colored\dante\arrow5.png)").string().c_str()); //ColoredDanteArrow5
        g_SpriteDescs.emplace_back((textureRoot / R"(colored\dante\center.png)").string().c_str()); //ColoredDanteCenter

        g_SpriteDescs.emplace_back((textureRoot / R"(colored\vergil\panelsactive\panel1.png)").string().c_str()); //ColoredVergilPanel1Active
        g_SpriteDescs.emplace_back((textureRoot / R"(colored\vergil\panelsactive\panel2.png)").string().c_str()); //ColoredVergilPanel2Active
        g_SpriteDescs.emplace_back((textureRoot / R"(colored\vergil\panelsactive\panel3.png)").string().c_str()); //ColoredVergilPanel3Active
        g_SpriteDescs.emplace_back((textureRoot / R"(colored\vergil\panelsactive\panel4.png)").string().c_str()); //ColoredVergilPanel4Active
        g_SpriteDescs.emplace_back((textureRoot / R"(colored\vergil\panelsactive\panel5.png)").string().c_str()); //ColoredVergilPanel5Active

        g_SpriteDescs.emplace_back((textureRoot / R"(colored\vergil\panelsinactive\panel1.png)").string().c_str()); //ColoredVergilPanel1Inactive
        g_SpriteDescs.emplace_back((textureRoot / R"(colored\vergil\panelsinactive\panel2.png)").string().c_str()); //ColoredVergilPanel2Inactive
        g_SpriteDescs.emplace_back((textureRoot / R"(colored\vergil\panelsinactive\panel3.png)").string().c_str()); //ColoredVergilPanel3Inactive
        g_SpriteDescs.emplace_back((textureRoot / R"(colored\vergil\panelsinactive\panel4.png)").string().c_str()); //ColoredVergilPanel4Inactive
        g_SpriteDescs.emplace_back((textureRoot / R"(colored\vergil\panelsinactive\panel5.png)").string().c_str()); //ColoredVergilPanel5Inactive

        g_SpriteDescs.emplace_back((textureRoot / R"(colored\vergil\arrow1.png)").string().c_str()); //ColoredVergilArrow1
        g_SpriteDescs.emplace_back((textureRoot / R"(colored\vergil\arrow2.png)").string().c_str()); //ColoredVergilArrow2
        g_SpriteDescs.emplace_back((textureRoot / R"(colored\vergil\arrow3.png)").string().c_str()); //ColoredVergilArrow3
        g_SpriteDescs.emplace_back((textureRoot / R"(colored\vergil\arrow4.png)").string().c_str()); //ColoredVergilArrow4
        g_SpriteDescs.emplace_back((textureRoot / R"(colored\vergil\arrow5.png)").string().c_str()); //ColoredVergilArrow5
        g_SpriteDescs.emplace_back((textureRoot / R"(colored\vergil\center.png)").string().c_str()); //ColoredVergilCenter

        g_SpriteDescs.emplace_back((textureRoot / R"(colorless\panelsactive\panel1.png)").string().c_str()); //ColorlessPanel1Active
        g_SpriteDescs.emplace_back((textureRoot / R"(colorless\panelsactive\panel2.png)").string().c_str()); //ColorlessPanel2Active
        g_SpriteDescs.emplace_back((textureRoot / R"(colorless\panelsactive\panel3.png)").string().c_str()); //ColorlessPanel3Active
        g_SpriteDescs.emplace_back((textureRoot / R"(colorless\panelsactive\panel4.png)").string().c_str()); //ColorlessPanel4Active
        g_SpriteDescs.emplace_back((textureRoot / R"(colorless\panelsactive\panel5.png)").string().c_str()); //ColorlessPanel5Active

        g_SpriteDescs.emplace_back((textureRoot / R"(colorless\panelsinactive\panel1.png)").string().c_str()); //ColorlessPanel1Inactive
        g_SpriteDescs.emplace_back((textureRoot / R"(colorless\panelsinactive\panel2.png)").string().c_str()); //ColorlessPanel2Inactive
        g_SpriteDescs.emplace_back((textureRoot / R"(colorless\panelsinactive\panel3.png)").string().c_str()); //ColorlessPanel3Inactive
        g_SpriteDescs.emplace_back((textureRoot / R"(colorless\panelsinactive\panel4.png)").string().c_str()); //ColorlessPanel4Inactive
        g_SpriteDescs.emplace_back((textureRoot / R"(colorless\panelsinactive\panel5.png)").string().c_str()); //ColorlessPanel5Inactive

        g_SpriteDescs.emplace_back((textureRoot / R"(colorless\arrow1.png)").string().c_str()); //ColorlessArrow1
        g_SpriteDescs.emplace_back((textureRoot / R"(colorless\arrow2.png)").string().c_str()); //ColorlessArrow2
        g_SpriteDescs.emplace_back((textureRoot / R"(colorless\arrow3.png)").string().c_str()); //ColorlessArrow3
        g_SpriteDescs.emplace_back((textureRoot / R"(colorless\arrow4.png)").string().c_str()); //ColorlessArrow4
        g_SpriteDescs.emplace_back((textureRoot / R"(colorless\arrow5.png)").string().c_str()); //ColorlessArrow5
        g_SpriteDescs.emplace_back((textureRoot / R"(colorless\center.png)").string().c_str()); //ColorlessCenter

        g_SpriteDescs.emplace_back((textureRoot / R"(melee\active\rebellion-awakened.png)").string().c_str()); //RebellionAwakenedActive
        g_SpriteDescs.emplace_back((textureRoot / R"(melee\active\rebellion-dormant.png)").string().c_str()); //RebellionDormantActive
        g_SpriteDescs.emplace_back((textureRoot / R"(melee\active\cerberus.png)").string().c_str()); //CerberusActive
        g_SpriteDescs.emplace_back((textureRoot / R"(melee\active\agnirudra.png)").string().c_str()); //AgniRudraActive
        g_SpriteDescs.emplace_back((textureRoot / R"(melee\active\nevan.png)").string().c_str()); //NevanActive
        g_SpriteDescs.emplace_back((textureRoot / R"(melee\active\beowulf.png)").string().c_str()); //BeowulfActive
        g_SpriteDescs.emplace_back((textureRoot / R"(melee\active\forceedge-dante.png)").string().c_str()); //ForceEdgeActive

        g_SpriteDescs.emplace_back((textureRoot / R"(guns\active\ebonyivory.png)").string().c_str()); //EbonyIvoryActive
        g_SpriteDescs.emplace_back((textureRoot / R"(guns\active\shotgun.png)").string().c_str()); //ShotgunActive
        g_SpriteDescs.emplace_back((textureRoot / R"(guns\active\artemis.png)").string().c_str()); //ArtemisActive
        g_SpriteDescs.emplace_back((textureRoot / R"(guns\active\spiral.png)").string().c_str()); //SpiralActive
        g_SpriteDescs.emplace_back((textureRoot / R"(guns\active\kalina.png)").string().c_str()); //KalinaActive

        g_SpriteDescs.emplace_back((textureRoot / R"(melee\active\yamato.png)").string().c_str()); //YamatoActive
        g_SpriteDescs.emplace_back((textureRoot / R"(melee\active\forceedge-vergil.png)").string().c_str()); //YamatoForceEdgeActive

        g_SpriteDescs.emplace_back((textureRoot / R"(melee\inactive\rebellion-awakened.png)").string().c_str()); //RebellionAwakenedInactive
        g_SpriteDescs.emplace_back((textureRoot / R"(melee\inactive\rebellion-dormant.png)").string().c_str()); //RebellionDormantInactive
        g_SpriteDescs.emplace_back((textureRoot / R"(melee\inactive\cerberus.png)").string().c_str()); //CerberusInactive
        g_SpriteDescs.emplace_back((textureRoot / R"(melee\inactive\agnirudra.png)").string().c_str()); //AgniRudraInactive
        g_SpriteDescs.emplace_back((textureRoot / R"(melee\inactive\nevan.png)").string().c_str()); //NevanInactive
        g_SpriteDescs.emplace_back((textureRoot / R"(melee\inactive\beowulf.png)").string().c_str()); //BeowulfInactive
        g_SpriteDescs.emplace_back((textureRoot / R"(melee\inactive\forceedge-dante.png)").string().c_str()); //ForceEdgeInactive

        g_SpriteDescs.emplace_back((textureRoot / R"(guns\inactive\ebonyivory.png)").string().c_str()); //EbonyIvoryInactive
        g_SpriteDescs.emplace_back((textureRoot / R"(guns\inactive\shotgun.png)").string().c_str()); //ShotgunInactive
        g_SpriteDescs.emplace_back((textureRoot / R"(guns\inactive\artemis.png)").string().c_str()); //ArtemisInactive
        g_SpriteDescs.emplace_back((textureRoot / R"(guns\inactive\spiral.png)").string().c_str()); //SpiralInactive
        g_SpriteDescs.emplace_back((textureRoot / R"(guns\inactive\kalina.png)").string().c_str()); //KalinaInactive

        g_SpriteDescs.emplace_back((textureRoot / R"(melee\inactive\yamato.png)").string().c_str()); //YamatoInactive
        g_SpriteDescs.emplace_back((textureRoot / R"(melee\inactive\forceedge-vergil.png)").string().c_str()); //YamatoForceEdgeInactive
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

                spriteIds.push_back((size_t)GetWeaponTextureID(weaponIDs[i], true)); // 7th: The active weapon animation duplicates
            }
        }

        return spriteIds;
    }

    std::shared_ptr<Graphics::Texture2DArrayD3D11> WeaponWheel::s_pTextureArray = {};

    WeaponWheel::WeaponWheel(
        ID3D11Device* pD3D11Device,
        ID3D11DeviceContext* pD3D11DeviceContext,
        UINT width,
        UINT height,
        std::vector<WeaponIDs> weapons,
        WheelThemes themeID)
        : m_pD3D11Device(pD3D11Device), m_pD3D11DeviceContext(pD3D11DeviceContext), m_Width(width), m_Height(height),
        m_Weapons(weapons), m_ThemeID(themeID)
    {
        // Load the sprite descriptors only once
        if (g_SpriteDescs.size() == 0)
            LoadSpriteDescs();

        // Initialize the texture array for the sprites only once
        if (!s_pTextureArray) {
            std::vector<std::string> paths;
            paths.reserve(g_SpriteDescs.size());

            for (const auto& sDesc : g_SpriteDescs)
                paths.push_back(sDesc.GetTexturePath());

            s_pTextureArray = std::make_shared<Graphics::Texture2DArrayD3D11>(paths, m_pD3D11Device);
        }

        m_SpriteIndices.UpdateSprites(m_ThemeID, m_Weapons);

        m_pSpriteBatch = std::make_unique<Graphics::BatchedSprites>(m_pD3D11Device, m_Width, m_Height, g_SpriteDescs, s_pTextureArray, m_SpriteIndices.GetSpriteIds());

        SetWeaponsTranslations();
        InitializeAnimations();
    }

    WeaponWheel::~WeaponWheel()
    {}

    void WeaponWheel::SetWeapons(std::vector<WeaponIDs> weapons)
    {
        m_PreviousWeapons = m_Weapons;
        m_Weapons = weapons;

        m_SpriteIndices.UpdateSprites(m_ThemeID, m_Weapons);

        m_pSpriteBatch->ChangeSprites(m_pD3D11Device, g_SpriteDescs, m_SpriteIndices.GetSpriteIds(), s_pTextureArray);

        SetWeaponsTranslations();

        //bool shouldUnhide = unhideWheel && !IsVisible();
        //
        //if (shouldUnhide)
        //    m_AnimData.ResetHidingAnimations();

        m_AnimData.ResetHidingAnimations();

        UpdateCurrentWheelVisibility(SpriteUpdateModes::FullyVisible);
    }

    void WeaponWheel::SetWheelTheme(WheelThemes themeID)
    {
        m_ThemeID = themeID;

        m_SpriteIndices.UpdateSprites(m_ThemeID, m_Weapons);

        m_pSpriteBatch->ChangeSprites(m_pD3D11Device, g_SpriteDescs, m_SpriteIndices.GetSpriteIds(), s_pTextureArray);

        SetWeaponsTranslations();

        //bool shouldUnhide = unhideWheel && !IsVisible();
        //
        //if (shouldUnhide)
        //    m_AnimData.ResetHidingAnimations();

        m_AnimData.ResetHidingAnimations();

        UpdateCurrentWheelVisibility(SpriteUpdateModes::FullyVisible);
    }

    void WeaponWheel::SetActiveSlot(size_t slot)
    {
        m_PreviousActiveSlot = m_CurrentActiveSlot;
        m_CurrentActiveSlot = slot;

        //bool shouldUnhide = unhideWheel && !IsVisible();
        //
        //if (shouldUnhide)
        //    m_AnimData.ResetHidingAnimations();

        m_AnimData.ResetHidingAnimations();
        m_AnimData.ResetSwitchingAnimations();

        UpdateCurrentWheelVisibility(SpriteUpdateModes::FullyVisible);
    }

    void WeaponWheel::ToggleAlwaysVisible(std::optional<bool> hide)
    {
        if (hide.has_value())
            m_AlwaysVisible = *hide;
        else
            m_AlwaysVisible = !m_AlwaysVisible;

        // Unhide the wheel
        if (m_AlwaysVisible == true)
        {
            // Reset the animations for hiding, so if they are in the middle of running they reset the state
            //m_AnimData.ResetHidingAnimations();

            for (size_t i = 0; i < m_Weapons.size(); i++)
            {
                // Inactive slots opacity
                if (i != m_CurrentActiveSlot)
                {
                    m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetInactiveWeaponIdx(i), 1.0f);
                    m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetInactivePanelIdx(i), 1.0f);
                }
            }

            m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetArrowIdx(m_CurrentActiveSlot), 1.0f);
            m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetActivePanelIdx(m_CurrentActiveSlot), 1.0f);
            m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetActiveWeaponIdx(m_CurrentActiveSlot), 1.0f);

            m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetCenterPieceIdx(), 1.0f);
        }
    }

	void WeaponWheel::ToggleAnalogSwitchingUI(std::optional<bool> state, bool instant) {
		// If given a state, will set it, if not toggles the mode
		m_AnalogSwitching = state.value_or(!m_AnalogSwitching);

		// Make sure the wheel is not invisible when toggling on
		if (m_AnalogSwitching) {
			if (instant) {
				// Skip the fade-in animation and show instantly
				m_AnimData.ResetASUnhidingAnimations(false);
				UpdateCurrentWheelVisibility(SpriteUpdateModes::FullyVisible);

				// Instantly show the analog arrows at their target opacity
				for (size_t i = 0; i < m_Weapons.size(); i++) {
					if (i != m_CurrentActiveSlot) {
						m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetArrowIdx(i), s_AnalogArrowsOpacity);
					}
				}

				m_AnimData.ResetHidingAnimations(false);
			} else {
				// Use fade-in animation as before
				m_AnimData.ResetASUnhidingAnimations();
				m_AnimData.ResetHidingAnimations(false);
			}
		} else {
			// Fade out behavior remains the same regardless of instant parameter
			m_AnimData.ResetASHidingAnimations();
			m_AnimData.ResetHidingAnimations();

			m_AnimData.ResetASUnhidingAnimations(false);
		}
	}

    bool WeaponWheel::IsVisible()
    {
        // If the timer for the hiding is already hit
        bool hidingTimeoutHit = m_AnimData.HideTimerMs > s_FadeDelay;

        return !hidingTimeoutHit || m_AlwaysVisible || m_AnalogSwitching;
    }

    void WeaponWheel::UpdateSwitchButtonHeldEnough(bool isButtonDown, double ms) {
		if (isButtonDown) {
			if (!m_wasButtonDown) {
				// Button just pressed, start timer
				m_buttonPressStartTime = std::chrono::steady_clock::now();
				m_wasButtonDown = true;
				m_heldLongEnough = false;
			} else {
				// Check if button has been held long enough
				if (!m_heldLongEnough) {
					auto currentTime = std::chrono::steady_clock::now();
					auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - m_buttonPressStartTime);
					if (elapsed.count() >= ms) {
						m_heldLongEnough = true;
					}
				}
			}
		} else {
			// Button not pressed, reset timer
			m_wasButtonDown = false;
			m_heldLongEnough = false;
		}
    }

    void WeaponWheel::OnUpdate(double ts)
    {
        // Hiding animations

        bool shouldKeepVisible = m_AnalogSwitching || m_AlwaysVisible;

        if (!shouldKeepVisible && !m_AnimData.WheelFadeOut.AlreadyTriggeredFlag && m_AnimData.HideTimerMs >= s_FadeDelay)
        {
            m_AnimData.WheelFadeOut.RunFlag = true;
            m_AnimData.WheelFadeOut.AlreadyTriggeredFlag = true;
        }

        if (!shouldKeepVisible && !m_AnimData.ArrowFadeOut.AlreadyTriggeredFlag && m_AnimData.HideTimerMs >= s_FadeDelay)
        {
            m_AnimData.ArrowFadeOut.RunFlag = true;
            m_AnimData.ArrowFadeOut.AlreadyTriggeredFlag = true;
        }

        if (!shouldKeepVisible && !m_AnimData.ActiveWeaponFadeOut.AlreadyTriggeredFlag && m_AnimData.HideTimerMs >= s_FadeDelay)
        {
            m_AnimData.ActiveWeaponFadeOut.RunFlag = true;
            m_AnimData.ActiveWeaponFadeOut.AlreadyTriggeredFlag = true;
        }

        // -----------------

        // Switching Animations

        if (!m_AnimData.WeaponSwitchScaleUp.AlreadyTriggeredFlag)
        {
            m_AnimData.WeaponSwitchScaleUp.RunFlag = true;
            m_AnimData.WeaponSwitchScaleUp.AlreadyTriggeredFlag = true;
        }

        if (!m_AnimData.WeaponSwitchBrightness.AlreadyTriggeredFlag)
        {
            m_AnimData.WeaponSwitchBrightness.RunFlag = false;
            m_AnimData.WeaponSwitchBrightness.AlreadyTriggeredFlag = true;
        }

        if (!m_AnimData.ArrowBrightness.AlreadyTriggeredFlag)
        {
            m_AnimData.ArrowBrightness.RunFlag = true;
            m_AnimData.ArrowBrightness.AlreadyTriggeredFlag = true;
        }

        // -----------------

        // Analog switching unhiding animations

        if (m_AnalogSwitching && !m_AnimData.AnalogArrowsFadeIn.AlreadyTriggeredFlag)
        {
            m_AnimData.AnalogArrowsFadeIn.RunFlag = true;
            m_AnimData.AnalogArrowsFadeIn.AlreadyTriggeredFlag = true;
        }

        // -----------------

        // Analog switching hiding animations

        if (!m_AnalogSwitching && !m_AnimData.AnalogArrowsFadeOut.AlreadyTriggeredFlag)
        {
            m_AnimData.AnalogArrowsFadeOut.RunFlag = true;
            m_AnimData.AnalogArrowsFadeOut.AlreadyTriggeredFlag = true;
        }

        // -----------------

        if (m_AnimData.WheelFadeOut.RunFlag)
            m_AnimData.WheelFadeOut.pAnimation->OnUpdate(ts);

        if (m_AnimData.ArrowFadeOut.RunFlag)
            m_AnimData.ArrowFadeOut.pAnimation->OnUpdate(ts);

        if (m_AnimData.ArrowBrightness.RunFlag)
            m_AnimData.ArrowBrightness.pAnimation->OnUpdate(ts);

        if (m_AnimData.ActiveWeaponFadeOut.RunFlag)
            m_AnimData.ActiveWeaponFadeOut.pAnimation->OnUpdate(ts);

        if (m_AnimData.WeaponSwitchScaleUp.RunFlag)
            m_AnimData.WeaponSwitchScaleUp.pAnimation->OnUpdate(ts);

        if (m_AnimData.WeaponSwitchBrightness.RunFlag)
            m_AnimData.WeaponSwitchBrightness.pAnimation->OnUpdate(ts);

        if (m_AnimData.AnalogArrowsFadeIn.RunFlag)
            m_AnimData.AnalogArrowsFadeIn.pAnimation->OnUpdate(ts);

        if (m_AnimData.AnalogArrowsFadeOut.RunFlag)
            m_AnimData.AnalogArrowsFadeOut.pAnimation->OnUpdate(ts);

        // Update timers
        m_AnimData.AdvanceTimers(ts);
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
        // The animation descriptions are set here

        // Wheel fading out animation
        {
            m_AnimData.WheelFadeOut.pAnimation =
                std::make_unique<GenericAnimation>(s_WheelFadeoutDur, &m_AnimData.WheelFadeOut);

            // Before the animation starts
            m_AnimData.WheelFadeOut.pAnimation->SetOnStart([this](GenericAnimation* pAnim)
                {
                    // Ensure the state is set to normal
                    UpdateCurrentWheelVisibility(SpriteUpdateModes::FullyVisible);
                });

            // On update
            m_AnimData.WheelFadeOut.pAnimation->SetOnUpdate([this](GenericAnimation* pAnim)
                {
                    const auto progress = pAnim->GetProgressNormalized();

                    for (size_t i = 0; i < m_Weapons.size(); i++)
                    {
                        // Inactive slots opacity
                        if (i != m_CurrentActiveSlot)
                        {
                            m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetInactiveWeaponIdx(i), 1.0f - progress);
                            m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetInactivePanelIdx(i), 1.0f - progress);
                        }
                        else // Active slots opacity
                        {
                            m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetArrowIdx(i), 1.0f - progress);
                            m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetActivePanelIdx(i), 1.0f - progress);
                        }
                    }

                    m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetCenterPieceIdx(), 1.0f - progress);
                });

            // After the animation ends
            m_AnimData.WheelFadeOut.pAnimation->SetOnEnd([this](GenericAnimation* pAnim)
                {
                    AnimState* pAnimState = reinterpret_cast<AnimState*>(pAnim->GetCustomDataPointer());

                    pAnimState->RunFlag = false;
                });

            // When reset
            m_AnimData.WheelFadeOut.pAnimation->SetOnReset([this](GenericAnimation* pAnim)
                {
                    AnimState* pAnimState = reinterpret_cast<AnimState*>(pAnim->GetCustomDataPointer());

                    pAnimState->RunFlag = false;

                    UpdateCurrentWheelVisibility(SpriteUpdateModes::FullyVisible);
                });
        }

        // Selection arrow fading out animation
        {
            m_AnimData.ArrowFadeOut.pAnimation =
                std::make_unique<GenericAnimation>(s_ArrowFadeAnimDur, &m_AnimData.ArrowFadeOut);

            // Before the animation starts
            m_AnimData.ArrowFadeOut.pAnimation->SetOnStart([this](GenericAnimation* pAnim)
                {
                    // Ensure the state is set to normal
                    m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetArrowIdx(m_CurrentActiveSlot), 1.0f);
                });

            // On update
            m_AnimData.ArrowFadeOut.pAnimation->SetOnUpdate([this](GenericAnimation* pAnim)
                {
                    const auto progress = pAnim->GetProgressNormalized();

                    m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetArrowIdx(m_CurrentActiveSlot), 1.0f - progress);
                });

            // After the animation ends
            m_AnimData.ArrowFadeOut.pAnimation->SetOnEnd([this](GenericAnimation* pAnim)
                {
                    AnimState* pAnimState = reinterpret_cast<AnimState*>(pAnim->GetCustomDataPointer());

                    pAnimState->RunFlag = false;

                    m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetArrowIdx(m_CurrentActiveSlot), 0.0f);
                });

            // When reset
            m_AnimData.ArrowFadeOut.pAnimation->SetOnReset([this](GenericAnimation* pAnim)
                {
                    AnimState* pAnimState = reinterpret_cast<AnimState*>(pAnim->GetCustomDataPointer());

                    pAnimState->RunFlag = false;

                    m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetArrowIdx(m_CurrentActiveSlot), 1.0f);
                });
        }

        // Selection arrow brightness animation when switching weapons
        {
            m_AnimData.ArrowBrightness.pAnimation =
                std::make_unique<GenericAnimation>(s_SwitchBrightnessAnimDur, &m_AnimData.ArrowBrightness);

            // Before the animation starts
            m_AnimData.ArrowBrightness.pAnimation->SetOnStart([this](GenericAnimation* pAnim)
                {
                    // Ensure the active weapon is at normal brightness
                    m_pSpriteBatch->SetBrightness(m_SpriteIndices.GetArrowIdx(m_CurrentActiveSlot), 1.0f);
                });

            // On update
            m_AnimData.ArrowBrightness.pAnimation->SetOnUpdate([this](GenericAnimation* pAnim)
                {
                    const auto progress = pAnim->GetProgressNormalized();

                    m_pSpriteBatch->SetBrightness(m_SpriteIndices.GetArrowIdx(m_CurrentActiveSlot), progress * 5.18f);
                });

            // After the animation ends
            m_AnimData.ArrowBrightness.pAnimation->SetOnEnd([this](GenericAnimation* pAnim)
                {
                    AnimState* pAnimState = reinterpret_cast<AnimState*>(pAnim->GetCustomDataPointer());

                    pAnimState->RunFlag = false;

                    m_pSpriteBatch->SetBrightness(m_SpriteIndices.GetArrowIdx(m_CurrentActiveSlot), 1.0f);
                });

            // When reset
            m_AnimData.ArrowBrightness.pAnimation->SetOnReset([this](GenericAnimation* pAnim)
                {
                    AnimState* pAnimState = reinterpret_cast<AnimState*>(pAnim->GetCustomDataPointer());

                    pAnimState->RunFlag = false;

                    m_pSpriteBatch->SetBrightness(m_SpriteIndices.GetArrowIdx(m_CurrentActiveSlot), 1.0f);
                    UpdateCurrentWheelVisibility(SpriteUpdateModes::FullyVisible);
                });
        }

        // Active weapon sprite fading out animation
        {
            m_AnimData.ActiveWeaponFadeOut.pAnimation =
                std::make_unique<GenericAnimation>(s_ActiveWeaponFadeoutDur, &m_AnimData.ActiveWeaponFadeOut);

            // Before the animation starts
            m_AnimData.ActiveWeaponFadeOut.pAnimation->SetOnStart([this](GenericAnimation* pAnim)
                {
                    // Ensure the active weapon is at full opacity
                    m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetActiveWeaponIdx(m_CurrentActiveSlot), 1.0f);
                });

            // On update
            m_AnimData.ActiveWeaponFadeOut.pAnimation->SetOnUpdate([this](GenericAnimation* pAnim)
                {
                    const auto progress = pAnim->GetProgressNormalized();

                    m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetActiveWeaponIdx(m_CurrentActiveSlot), 1.0f - progress);
                });

            // After the animation ends
            m_AnimData.ActiveWeaponFadeOut.pAnimation->SetOnEnd([this](GenericAnimation* pAnim)
                {
                    AnimState* pAnimState = reinterpret_cast<AnimState*>(pAnim->GetCustomDataPointer());

                    pAnimState->RunFlag = false;
                });

            // When reset
            m_AnimData.ActiveWeaponFadeOut.pAnimation->SetOnReset([this](GenericAnimation* pAnim)
                {
                    AnimState* pAnimState = reinterpret_cast<AnimState*>(pAnim->GetCustomDataPointer());

                    pAnimState->RunFlag = false;

                    m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetActiveWeaponIdx(m_CurrentActiveSlot), 1.0f);
                });
        }

        // Weapon sprite brightness animation when switching to weapon
        {
            m_AnimData.WeaponSwitchBrightness.pAnimation =
                std::make_unique<GenericAnimation>(s_SwitchBrightnessAnimDur, &m_AnimData.WeaponSwitchBrightness);

            // Before the animation starts
            m_AnimData.WeaponSwitchBrightness.pAnimation->SetOnStart([this](GenericAnimation* pAnim)
                {
                    // Ensure the active weapon is at normal brightness
                    m_pSpriteBatch->SetBrightness(m_SpriteIndices.GetActiveWeaponIdx(m_CurrentActiveSlot), 1.0f);
                });

            // On update
            m_AnimData.WeaponSwitchBrightness.pAnimation->SetOnUpdate([this](GenericAnimation* pAnim)
                {
                    const auto progress = pAnim->GetProgressNormalized();

                    m_pSpriteBatch->SetBrightness(m_SpriteIndices.GetActiveWeaponIdx(m_CurrentActiveSlot), progress * 3.18f);
                });

            // After the animation ends
            m_AnimData.WeaponSwitchBrightness.pAnimation->SetOnEnd([this](GenericAnimation* pAnim)
                {
                    AnimState* pAnimState = reinterpret_cast<AnimState*>(pAnim->GetCustomDataPointer());

                    pAnimState->RunFlag = false;

                    m_pSpriteBatch->SetBrightness(m_SpriteIndices.GetActiveWeaponIdx(m_CurrentActiveSlot), 1.0f);
                });

            // When reset
            m_AnimData.WeaponSwitchBrightness.pAnimation->SetOnReset([this](GenericAnimation* pAnim)
                {
                    AnimState* pAnimState = reinterpret_cast<AnimState*>(pAnim->GetCustomDataPointer());

                    pAnimState->RunFlag = false;

                    m_pSpriteBatch->SetBrightness(m_SpriteIndices.GetActiveWeaponIdx(m_CurrentActiveSlot), 1.0f);
                });
        }

        // Weapon scale animation when switching to weapon
        {
            m_AnimData.WeaponSwitchScaleUp.pAnimation =
                std::make_unique<GenericAnimation>(s_SwitchScaleAnimDur, &m_AnimData.WeaponSwitchScaleUp);

            // Before the animation starts
            m_AnimData.WeaponSwitchScaleUp.pAnimation->SetOnStart([this](GenericAnimation* pAnim)
                {
                    m_pSpriteBatch->SetOpacity(
                        m_SpriteIndices.GetActiveWeaponAnimationDupeIdx(m_CurrentActiveSlot), 0.0f);

                    m_pSpriteBatch->SetBrightness(
                        m_SpriteIndices.GetActiveWeaponAnimationDupeIdx(m_CurrentActiveSlot), 1.0f);

                    m_pSpriteBatch->ScaleTo(
                        m_SpriteIndices.GetActiveWeaponAnimationDupeIdx(m_CurrentActiveSlot), glm::vec3(1.0f));
                });

            // On update
            m_AnimData.WeaponSwitchScaleUp.pAnimation->SetOnUpdate([this](GenericAnimation* pAnim)
                {
                    const auto progress = pAnim->GetProgressNormalized();

                    m_pSpriteBatch->SetOpacity(
                        m_SpriteIndices.GetActiveWeaponAnimationDupeIdx(m_CurrentActiveSlot), progress * 0.9f);

                    m_pSpriteBatch->SetBrightness(
                        m_SpriteIndices.GetActiveWeaponAnimationDupeIdx(m_CurrentActiveSlot), progress * 3.18f);

                    m_pSpriteBatch->ScaleTo(
                        m_SpriteIndices.GetActiveWeaponAnimationDupeIdx(m_CurrentActiveSlot),
                        s_MeleeActiveSlotTransforms[m_CurrentActiveSlot].Scale * float(progress) * 1.5f);
                });

            // After the animation ends
            m_AnimData.WeaponSwitchScaleUp.pAnimation->SetOnEnd([this](GenericAnimation* pAnim)
                {
                    AnimState* pAnimState = reinterpret_cast<AnimState*>(pAnim->GetCustomDataPointer());

                    pAnimState->RunFlag = false;

                    m_pSpriteBatch->SetOpacity(
                        m_SpriteIndices.GetActiveWeaponAnimationDupeIdx(m_CurrentActiveSlot), 0.0f);

                    m_pSpriteBatch->SetBrightness(
                        m_SpriteIndices.GetActiveWeaponAnimationDupeIdx(m_CurrentActiveSlot), 1.0f);

                    m_pSpriteBatch->ScaleTo(
                        m_SpriteIndices.GetActiveWeaponAnimationDupeIdx(m_CurrentActiveSlot), glm::vec3(0.5));
                });

            // When reset
            m_AnimData.WeaponSwitchScaleUp.pAnimation->SetOnReset([this](GenericAnimation* pAnim)
                {
                    AnimState* pAnimState = reinterpret_cast<AnimState*>(pAnim->GetCustomDataPointer());

                    pAnimState->RunFlag = false;

                    m_pSpriteBatch->SetOpacity(
                        m_SpriteIndices.GetActiveWeaponAnimationDupeIdx(m_CurrentActiveSlot), 0.0f);

                    m_pSpriteBatch->SetBrightness(
                        m_SpriteIndices.GetActiveWeaponAnimationDupeIdx(m_CurrentActiveSlot), 1.0f);

                    m_pSpriteBatch->ScaleTo(
                        m_SpriteIndices.GetActiveWeaponAnimationDupeIdx(m_CurrentActiveSlot), glm::vec3(1.0f));
                });
        }

        // Analog weapon selection arrow fading in animation
        {
            m_AnimData.AnalogArrowsFadeIn.pAnimation =
                std::make_unique<GenericAnimation>(s_AnalogArrowFadInAnimDur, &m_AnimData.AnalogArrowsFadeIn);

            // Before the animation starts
            m_AnimData.AnalogArrowsFadeIn.pAnimation->SetOnStart([this](GenericAnimation* pAnim)
                {
                    // Ensure the state is set to normal
                    for (size_t i = 0; i < m_Weapons.size(); i++) {
                        if (i != m_CurrentActiveSlot) {
                            m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetArrowIdx(i), 0.05f);
                        }
                    }
                });

            // On update
            m_AnimData.AnalogArrowsFadeIn.pAnimation->SetOnUpdate([this](GenericAnimation* pAnim)
                {
                    const auto progress = pAnim->GetProgressNormalized();

                    for (size_t i = 0; i < m_Weapons.size(); i++) {
                        if (i != m_CurrentActiveSlot) {
                            m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetArrowIdx(i), std::min(0.0f + float(progress), s_AnalogArrowsOpacity));
                        }
                    }
                });

            // After the animation ends
            m_AnimData.AnalogArrowsFadeIn.pAnimation->SetOnEnd([this](GenericAnimation* pAnim)
                {
                    AnimState* pAnimState = reinterpret_cast<AnimState*>(pAnim->GetCustomDataPointer());

                    pAnimState->RunFlag = false;

                    for (size_t i = 0; i < m_Weapons.size(); i++) {
                        if (i != m_CurrentActiveSlot) {
                            m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetArrowIdx(i), s_AnalogArrowsOpacity);
                        }
                    }
                });

            // When reset
            m_AnimData.AnalogArrowsFadeIn.pAnimation->SetOnReset([this](GenericAnimation* pAnim)
                {
                    AnimState* pAnimState = reinterpret_cast<AnimState*>(pAnim->GetCustomDataPointer());

                    pAnimState->RunFlag = false;

                    for (size_t i = 0; i < m_Weapons.size(); i++) {
                        if (i != m_CurrentActiveSlot) {
                            m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetArrowIdx(i), s_AnalogArrowsOpacity);
                        }
                    }
                });
        }

        // Analog weapon selection arrow fading out animation
        {
            m_AnimData.AnalogArrowsFadeOut.pAnimation =
                std::make_unique<GenericAnimation>(s_ArrowFadeAnimDur, &m_AnimData.AnalogArrowsFadeOut);

            // Before the animation starts
            m_AnimData.AnalogArrowsFadeOut.pAnimation->SetOnStart([this](GenericAnimation* pAnim)
                {
                    // Ensure the state is set to normal
                    for (size_t i = 0; i < m_Weapons.size(); i++) {
                        if (i != m_CurrentActiveSlot) {
                            m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetArrowIdx(i), s_AnalogArrowsOpacity);
                        }
                    }
                });

            // On update
            m_AnimData.AnalogArrowsFadeOut.pAnimation->SetOnUpdate([this](GenericAnimation* pAnim)
                {
                    const auto progress = pAnim->GetProgressNormalized();

                    for (size_t i = 0; i < m_Weapons.size(); i++) {
                        if (i != m_CurrentActiveSlot) {
                            m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetArrowIdx(i), s_AnalogArrowsOpacity - s_AnalogArrowsOpacity * float(progress));
                        }
                    }
                });

            // After the animation ends
            m_AnimData.AnalogArrowsFadeOut.pAnimation->SetOnEnd([this](GenericAnimation* pAnim)
                {
                    AnimState* pAnimState = reinterpret_cast<AnimState*>(pAnim->GetCustomDataPointer());

                    pAnimState->RunFlag = false;

                    for (size_t i = 0; i < m_Weapons.size(); i++) {
                        if (i != m_CurrentActiveSlot) {
                            m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetArrowIdx(i), 0.0f);
                        }
                    }
                });

            // When reset
            m_AnimData.AnalogArrowsFadeOut.pAnimation->SetOnReset([this](GenericAnimation* pAnim)
                {
                    AnimState* pAnimState = reinterpret_cast<AnimState*>(pAnim->GetCustomDataPointer());

                    pAnimState->RunFlag = false;

                    for (size_t i = 0; i < m_Weapons.size(); i++) {
                        if (i != m_CurrentActiveSlot) {
                            m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetArrowIdx(i), s_AnalogArrowsOpacity);
                        }
                    }
                });
        }
    }

    void WeaponWheel::SetWeaponsTranslations()
    {
        for (size_t i = 0; i < m_Weapons.size(); i++)
        {
            // Set the transformation for the inactive weapon sprites
            m_pSpriteBatch->SetTransform(
                m_SpriteIndices.GetInactiveWeaponIdx(i),
                s_MeleeInactiveSlotTransforms[i].Translation,
                s_WeaponNormalRotations[(size_t)m_Weapons[i]],
                s_WeaponNormalInactiveScales[(size_t)m_Weapons[i]]
            );

            // Set the transformation for the active weapon sprites
            m_pSpriteBatch->SetTransform(
                m_SpriteIndices.GetActiveWeaponIdx(i),
                s_MeleeActiveSlotTransforms[i].Translation,
                s_WeaponNormalRotations[(size_t)m_Weapons[i]],
                s_WeaponNormalActiveScales[(size_t)m_Weapons[i]]
            );

            // Set the transformation for the duplicated weapon sprites
            m_pSpriteBatch->SetTransform(
                m_SpriteIndices.GetActiveWeaponAnimationDupeIdx(i),
                s_MeleeActiveSlotTransforms[i].Translation,
                s_WeaponNormalRotations[(size_t)m_Weapons[i]],
                s_WeaponNormalActiveScales[(size_t)m_Weapons[i]]
            );

            // Hide the duplicated weapon sprites by default
            m_pSpriteBatch->SetOpacity(
                m_SpriteIndices.GetActiveWeaponAnimationDupeIdx(i),
                0.0f
            );
        }
    }

    void WeaponWheel::SetInactiveOpacity(float opacity)
    {
        for (size_t i = 0; i < m_Weapons.size(); i++)
        {
            if (i != m_CurrentActiveSlot)
                m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetInactiveWeaponIdx(i), opacity);
        }
    }

    void WeaponWheel::SetActiveWeaponOpacity(float opacity)
    {
        if (m_CurrentActiveSlot > 4) // 5 slots is max
            return;

        m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetActiveWeaponIdx(m_CurrentActiveSlot), opacity);
    }

    void WeaponWheel::UpdateCurrentWheelVisibility(SpriteUpdateModes mode)
    {
        // Before changing anything do this so if needed old state gets backed up

        // Center piece
        float centerPieceOpacity = 0.0f;

        // Current active slot
        float activePanelOpacity = 0.0f;
        float activeWeaponOpacity = 0.0f;
        float activeArrowVisibility = 0.0f;

        switch (mode)
        {
        //case SpriteUpdateModes::KeepVisibility:
        //{
        //    centerPieceOpacity = m_pSpriteBatch->GetOpacity((size_t)GetCenterTextureID(m_ThemeID));
        //
        //    activePanelOpacity = m_pSpriteBatch->GetOpacity((size_t)GetPanelTextureID(m_ThemeID, m_PreviousActiveSlot, true));
        //    activeWeaponOpacity = m_pSpriteBatch->GetOpacity((size_t)GetWeaponTextureID(m_PreviousWeapons[m_PreviousActiveSlot], true));
        //    activeArrowVisibility = m_pSpriteBatch->GetOpacity((size_t)GetArrowTextureID(m_ThemeID, m_PreviousActiveSlot));
        //} break;

        case SpriteUpdateModes::FullyVisible:
        {
            centerPieceOpacity = 1.0f;

            activePanelOpacity = 1.0f;
            activeWeaponOpacity = 1.0f;
            activeArrowVisibility = 1.0f;
        } break;

        case SpriteUpdateModes::FullyHide:
        {
            centerPieceOpacity = 0.0f;

            activePanelOpacity = 0.0f;
            activeWeaponOpacity = 0.0f;
            activeArrowVisibility = 0.0f;
        } break;

        default:
            break;
        }

        // Set the state to thee normal non active state
        for (size_t i = 0; i < m_Weapons.size(); i++) {
            float inactiveWeaponOpacity = 0.0f;
            float inactivePanelOpacity = 0.0f;

            switch (mode)
            {
                //case SpriteUpdateModes::KeepVisibility:
                //{
                //    if (i != m_PreviousActiveSlot)
                //    {
                //        inactiveWeaponOpacity = m_pSpriteBatch->GetOpacity((size_t)GetWeaponTextureID(m_PreviousWeapons[i], false));
                //        inactivePanelOpacity = m_pSpriteBatch->GetOpacity((size_t)GetPanelTextureID(m_ThemeID, i, false));
                //    }
                //    else
                //    {
                //        inactiveWeaponOpacity = m_pSpriteBatch->GetOpacity((size_t)GetWeaponTextureID(m_PreviousWeapons[i], true));
                //        inactivePanelOpacity = m_pSpriteBatch->GetOpacity((size_t)GetPanelTextureID(m_ThemeID, i, true));
                //    }
                //} break;

                case SpriteUpdateModes::FullyVisible:
                {
                    inactiveWeaponOpacity = 1.0f;
                    inactivePanelOpacity = 1.0f;
                } break;

                case SpriteUpdateModes::FullyHide:
                {
                    inactiveWeaponOpacity = 0.0f;
                    inactivePanelOpacity = 0.0f;
                } break;

                default:
                    break;
            }

            // Set inactive texture opacities
            m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetInactiveWeaponIdx(i), inactiveWeaponOpacity);
            m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetInactivePanelIdx(i), inactivePanelOpacity);

            // Hide all active textures that aren't related to the current active slot
            if (i != m_CurrentActiveSlot)
            {
                m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetActivePanelIdx(i), 0.0f);
                m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetActiveWeaponIdx(i), 0.0f);
                m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetActiveWeaponAnimationDupeIdx(i), 0.0f);
                m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetArrowIdx(i), 0.0f);
            }
        }

        // Set the opacity of the current slot's active weapon and panel to the opacity of the previous active slot
        m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetActivePanelIdx(m_CurrentActiveSlot), activePanelOpacity);
        m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetActiveWeaponIdx(m_CurrentActiveSlot), activeWeaponOpacity);

        // Set the center piece opacity
        m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetCenterPieceIdx(), centerPieceOpacity);

        // Hide the inactive weapon texture, inactive panel texture and duplicate weapon texture for the current active slot
        m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetInactiveWeaponIdx(m_CurrentActiveSlot), 0.0f);
        m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetInactivePanelIdx(m_CurrentActiveSlot), 0.0f);
        m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetActiveWeaponAnimationDupeIdx(m_CurrentActiveSlot), 0.0f);

        m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetArrowIdx(m_CurrentActiveSlot), activeArrowVisibility);
        m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetActivePanelIdx(m_CurrentActiveSlot), activePanelOpacity);
        m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetActiveWeaponIdx(m_CurrentActiveSlot), activeWeaponOpacity);

        // If in analog switching mode, make sure the other arrows are semi-visible
        if (m_AnalogSwitching)
        {
            for (size_t i = 0; i < m_Weapons.size(); i++)
            {
                if (i != m_CurrentActiveSlot)
                    m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetArrowIdx(i), 0.55f);
            }
        }

        // Consume the old trackers
        m_PreviousActiveSlot = m_CurrentActiveSlot;
        m_PreviousWeapons = m_Weapons;
    }

    void WeaponWheel::SpriteIndices::UpdateSprites(WheelThemes themeId, const std::vector<WeaponIDs>& weaponIds)
    {
        m_SpriteIds.clear();
        m_SpriteIds.reserve(weaponIds.size() * 6);

        // Add the needed sprite ids to the list and store the index of the id for each one
        {
            // Order matters here, back to front

            // 1st: The center piece
            {
                m_CenterPieceIdx = m_SpriteIds.size();
                m_SpriteIds.push_back((size_t)GetCenterTextureID(themeId));
            }

            // 2nd: The arrows
            {
                m_ArrowsIdx.clear();
                m_ArrowsIdx.reserve(weaponIds.size());
                for (size_t i = 0; i < weaponIds.size(); i++)
                {
                    if (i > 4)
                        break;

                    m_ArrowsIdx.push_back(m_SpriteIds.size());
                    m_SpriteIds.push_back((size_t)GetArrowTextureID(themeId, i));
                }
            }

            // 3rd: The inactive panels
            {
                m_InactivePanelsIdx.clear();
                m_InactivePanelsIdx.reserve(weaponIds.size());
                for (size_t i = 0; i < weaponIds.size(); i++)
                {
                    if (i > 4)
                        break;

                    m_InactivePanelsIdx.push_back(m_SpriteIds.size());
                    m_SpriteIds.push_back((size_t)GetPanelTextureID(themeId, i, false));
                }
            }

            // 4th: The active panels
            {
                m_ActivePanelsIdx.clear();
                m_ActivePanelsIdx.reserve(weaponIds.size());
                for (size_t i = 0; i < weaponIds.size(); i++)
                {
                    if (i > 4)
                        break;

                    m_ActivePanelsIdx.push_back(m_SpriteIds.size());
                    m_SpriteIds.push_back((size_t)GetPanelTextureID(themeId, i, true));
                }
            }

            // 5th: The inactive weapons
            {
                m_InactiveWeaponsIdx.clear();
                m_InactiveWeaponsIdx.reserve(weaponIds.size());
                for (size_t i = 0; i < weaponIds.size(); i++)
                {
                    if (i > 4)
                        break;

                    m_InactiveWeaponsIdx.push_back(m_SpriteIds.size());
                    m_SpriteIds.push_back((size_t)GetWeaponTextureID(weaponIds[i], false));
                }
            }

            // 6th: The active weapons
            {
                m_ActiveWeaponsIdx.clear();
                m_ActiveWeaponsIdx.reserve(weaponIds.size());
                for (size_t i = 0; i < weaponIds.size(); i++)
                {
                    if (i > 4)
                        break;

                    m_ActiveWeaponsIdx.push_back(m_SpriteIds.size());
                    m_SpriteIds.push_back((size_t)GetWeaponTextureID(weaponIds[i], true));
                }
            }

            // 7th: The active weapon animation duplicates
            {
                m_ActiveWeaponAnimationDupesIdx.clear();
                m_ActiveWeaponAnimationDupesIdx.reserve(weaponIds.size());
                for (size_t i = 0; i < weaponIds.size(); i++)
                {
                    if (i > 4)
                        break;

                    m_ActiveWeaponAnimationDupesIdx.push_back(m_SpriteIds.size());
                    m_SpriteIds.push_back((size_t)GetWeaponTextureID(weaponIds[i], true));
                }
            }
        }
    }
}
