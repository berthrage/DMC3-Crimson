#include "WeaponWheel.hpp"

#include "Sprite.hpp"

#include <filesystem>

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

namespace WW {
    void LoadSprites(std::vector <Graphics::Sprite>& sprites, std::vector <Graphics::Sprite>& weaponSprites)
    {
        sprites.reserve((size_t)TextureID::Size);
        weaponSprites.reserve((size_t)WeaponTextureID::Size);
    
        const std::filesystem::path textureRoot(R"(Crimson\assets\weaponwheel)");
    
        sprites.emplace_back((textureRoot / R"(colored\dante\panelsactive\panel1.png)").string().c_str()); //ColoredDantePanel1Active
        sprites.emplace_back((textureRoot / R"(colored\dante\panelsactive\panel2.png)").string().c_str()); //ColoredDantePanel2Active
        sprites.emplace_back((textureRoot / R"(colored\dante\panelsactive\panel3.png)").string().c_str()); //ColoredDantePanel3Active
        sprites.emplace_back((textureRoot / R"(colored\dante\panelsactive\panel4.png)").string().c_str()); //ColoredDantePanel4Active
        sprites.emplace_back((textureRoot / R"(colored\dante\panelsactive\panel5.png)").string().c_str()); //ColoredDantePanel5Active
    
        sprites.emplace_back((textureRoot / R"(colored\dante\panelsinactive\panel1.png)").string().c_str()); //ColoredDantePanel1Inactive
        sprites.emplace_back((textureRoot / R"(colored\dante\panelsinactive\panel2.png)").string().c_str()); //ColoredDantePanel2Inactive
        sprites.emplace_back((textureRoot / R"(colored\dante\panelsinactive\panel3.png)").string().c_str()); //ColoredDantePanel3Inactive
        sprites.emplace_back((textureRoot / R"(colored\dante\panelsinactive\panel4.png)").string().c_str()); //ColoredDantePanel4Inactive
        sprites.emplace_back((textureRoot / R"(colored\dante\panelsinactive\panel5.png)").string().c_str()); //ColoredDantePanel5Inactive
    
        sprites.emplace_back((textureRoot / R"(colored\dante\arrow1.png)").string().c_str()); //ColoredDanteArrow1
        sprites.emplace_back((textureRoot / R"(colored\dante\arrow2.png)").string().c_str()); //ColoredDanteArrow2
        sprites.emplace_back((textureRoot / R"(colored\dante\arrow3.png)").string().c_str()); //ColoredDanteArrow3
        sprites.emplace_back((textureRoot / R"(colored\dante\arrow4.png)").string().c_str()); //ColoredDanteArrow4
        sprites.emplace_back((textureRoot / R"(colored\dante\arrow5.png)").string().c_str()); //ColoredDanteArrow5
        sprites.emplace_back((textureRoot / R"(colored\dante\center.png)").string().c_str()); //ColoredDanteCenter
    
        sprites.emplace_back((textureRoot / R"(colored\vergil\panelsactive\panel1.png)").string().c_str()); //ColoredVergilPanel1Active
        sprites.emplace_back((textureRoot / R"(colored\vergil\panelsactive\panel2.png)").string().c_str()); //ColoredVergilPanel2Active
        sprites.emplace_back((textureRoot / R"(colored\vergil\panelsactive\panel3.png)").string().c_str()); //ColoredVergilPanel3Active
        sprites.emplace_back((textureRoot / R"(colored\vergil\panelsactive\panel4.png)").string().c_str()); //ColoredVergilPanel4Active
        sprites.emplace_back((textureRoot / R"(colored\vergil\panelsactive\panel5.png)").string().c_str()); //ColoredVergilPanel5Active
    
        sprites.emplace_back((textureRoot / R"(colored\vergil\panelsinactive\panel1.png)").string().c_str()); //ColoredVergilPanel1Inactive
        sprites.emplace_back((textureRoot / R"(colored\vergil\panelsinactive\panel2.png)").string().c_str()); //ColoredVergilPanel2Inactive
        sprites.emplace_back((textureRoot / R"(colored\vergil\panelsinactive\panel3.png)").string().c_str()); //ColoredVergilPanel3Inactive
        sprites.emplace_back((textureRoot / R"(colored\vergil\panelsinactive\panel4.png)").string().c_str()); //ColoredVergilPanel4Inactive
        sprites.emplace_back((textureRoot / R"(colored\vergil\panelsinactive\panel5.png)").string().c_str()); //ColoredVergilPanel5Inactive
    
        sprites.emplace_back((textureRoot / R"(colored\vergil\arrow1.png)").string().c_str()); //ColoredVergilArrow1
        sprites.emplace_back((textureRoot / R"(colored\vergil\arrow2.png)").string().c_str()); //ColoredVergilArrow2
        sprites.emplace_back((textureRoot / R"(colored\vergil\arrow3.png)").string().c_str()); //ColoredVergilArrow3
        sprites.emplace_back((textureRoot / R"(colored\vergil\arrow4.png)").string().c_str()); //ColoredVergilArrow4
        sprites.emplace_back((textureRoot / R"(colored\vergil\arrow5.png)").string().c_str()); //ColoredVergilArrow5
        sprites.emplace_back((textureRoot / R"(colored\vergil\center.png)").string().c_str()); //ColoredVergilCenter
    
        sprites.emplace_back((textureRoot / R"(colorless\panelsactive\panel1.png)").string().c_str()); //ColorlessPanel1Active
        sprites.emplace_back((textureRoot / R"(colorless\panelsactive\panel2.png)").string().c_str()); //ColorlessPanel2Active
        sprites.emplace_back((textureRoot / R"(colorless\panelsactive\panel3.png)").string().c_str()); //ColorlessPanel3Active
        sprites.emplace_back((textureRoot / R"(colorless\panelsactive\panel4.png)").string().c_str()); //ColorlessPanel4Active
        sprites.emplace_back((textureRoot / R"(colorless\panelsactive\panel5.png)").string().c_str()); //ColorlessPanel5Active
    
        sprites.emplace_back((textureRoot / R"(colorless\panelsinactive\panel1.png)").string().c_str()); //ColorlessPanel1Inactive
        sprites.emplace_back((textureRoot / R"(colorless\panelsinactive\panel2.png)").string().c_str()); //ColorlessPanel2Inactive
        sprites.emplace_back((textureRoot / R"(colorless\panelsinactive\panel3.png)").string().c_str()); //ColorlessPanel3Inactive
        sprites.emplace_back((textureRoot / R"(colorless\panelsinactive\panel4.png)").string().c_str()); //ColorlessPanel4Inactive
        sprites.emplace_back((textureRoot / R"(colorless\panelsinactive\panel5.png)").string().c_str()); //ColorlessPanel5Inactive
    
        sprites.emplace_back((textureRoot / R"(colorless\arrow1.png)").string().c_str()); //ColorlessArrow1
        sprites.emplace_back((textureRoot / R"(colorless\arrow2.png)").string().c_str()); //ColorlessArrow2
        sprites.emplace_back((textureRoot / R"(colorless\arrow3.png)").string().c_str()); //ColorlessArrow3
        sprites.emplace_back((textureRoot / R"(colorless\arrow4.png)").string().c_str()); //ColorlessArrow4
        sprites.emplace_back((textureRoot / R"(colorless\arrow5.png)").string().c_str()); //ColorlessArrow5
        sprites.emplace_back((textureRoot / R"(colorless\center.png)").string().c_str()); //ColorlessCenter

		weaponSprites.emplace_back((textureRoot / R"(melee\active\rebellion-awakened.png)").string().c_str()); //RebellionAwakenedActive
        weaponSprites.emplace_back((textureRoot / R"(melee\active\rebellion-dormant.png)").string().c_str()); //RebellionDormantActive
        weaponSprites.emplace_back((textureRoot / R"(melee\active\cerberus.png)").string().c_str()); //CerberusActive
        weaponSprites.emplace_back((textureRoot / R"(melee\active\agnirudra.png)").string().c_str()); //AgniRudraActive
        weaponSprites.emplace_back((textureRoot / R"(melee\active\nevan.png)").string().c_str()); //NevanActive
        weaponSprites.emplace_back((textureRoot / R"(melee\active\beowulf.png)").string().c_str()); //BeowulfActive
        weaponSprites.emplace_back((textureRoot / R"(melee\active\forceedge-dante.png)").string().c_str()); //ForceEdgeActive

        weaponSprites.emplace_back((textureRoot / R"(guns\active\ebonyivory.png)").string().c_str()); //EbonyIvoryActive
        weaponSprites.emplace_back((textureRoot / R"(guns\active\shotgun.png)").string().c_str()); //ShotgunActive
        weaponSprites.emplace_back((textureRoot / R"(guns\active\artemis.png)").string().c_str()); //ArtemisActive
        weaponSprites.emplace_back((textureRoot / R"(guns\active\spiral.png)").string().c_str()); //SpiralActive
        weaponSprites.emplace_back((textureRoot / R"(guns\active\kalina.png)").string().c_str()); //KalinaActive

        weaponSprites.emplace_back((textureRoot / R"(melee\active\yamato.png)").string().c_str()); //YamatoActive
        weaponSprites.emplace_back((textureRoot / R"(melee\active\forceedge-vergil.png)").string().c_str()); //YamatoForceEdgeActive

        weaponSprites.emplace_back((textureRoot / R"(melee\active\rebellion-awakened.png)").string().c_str()); //DuplicateRebellionAwakenedActive
        weaponSprites.emplace_back((textureRoot / R"(melee\active\rebellion-dormant.png)").string().c_str()); //DuplicateRebellionDormantActive
        weaponSprites.emplace_back((textureRoot / R"(melee\active\cerberus.png)").string().c_str()); //DuplicateCerberusActive
        weaponSprites.emplace_back((textureRoot / R"(melee\active\agnirudra.png)").string().c_str()); //DuplicateAgniRudraActive
        weaponSprites.emplace_back((textureRoot / R"(melee\active\nevan.png)").string().c_str()); //DuplicateNevanActive
        weaponSprites.emplace_back((textureRoot / R"(melee\active\beowulf.png)").string().c_str()); //DuplicateBeowulfActive
        weaponSprites.emplace_back((textureRoot / R"(melee\active\forceedge-dante.png)").string().c_str()); //DuplicateForceEdgeActive

        weaponSprites.emplace_back((textureRoot / R"(guns\active\ebonyivory.png)").string().c_str()); //DuplicateEbonyIvoryActive
        weaponSprites.emplace_back((textureRoot / R"(guns\active\shotgun.png)").string().c_str()); //DuplicateShotgunActive
        weaponSprites.emplace_back((textureRoot / R"(guns\active\artemis.png)").string().c_str()); //DuplicateArtemisActive
        weaponSprites.emplace_back((textureRoot / R"(guns\active\spiral.png)").string().c_str()); //DuplicateSpiralActive
        weaponSprites.emplace_back((textureRoot / R"(guns\active\kalina.png)").string().c_str()); //DuplicateKalinaActive

        weaponSprites.emplace_back((textureRoot / R"(melee\active\yamato.png)").string().c_str()); //DuplicateYamatoActive
        weaponSprites.emplace_back((textureRoot / R"(melee\active\forceedge-vergil.png)").string().c_str()); //DuplicateYamatoForceEdgeActive

        weaponSprites.emplace_back((textureRoot / R"(melee\inactive\rebellion-awakened.png)").string().c_str()); //RebellionAwakenedInactive
        weaponSprites.emplace_back((textureRoot / R"(melee\inactive\rebellion-dormant.png)").string().c_str()); //RebellionDormantInactive
        weaponSprites.emplace_back((textureRoot / R"(melee\inactive\cerberus.png)").string().c_str()); //CerberusInactive
        weaponSprites.emplace_back((textureRoot / R"(melee\inactive\agnirudra.png)").string().c_str()); //AgniRudraInactive
        weaponSprites.emplace_back((textureRoot / R"(melee\inactive\nevan.png)").string().c_str()); //NevanInactive
        weaponSprites.emplace_back((textureRoot / R"(melee\inactive\beowulf.png)").string().c_str()); //BeowulfInactive
        weaponSprites.emplace_back((textureRoot / R"(melee\inactive\forceedge-dante.png)").string().c_str()); //ForceEdgeInactive

        weaponSprites.emplace_back((textureRoot / R"(guns\inactive\ebonyivory.png)").string().c_str()); //EbonyIvoryInactive
        weaponSprites.emplace_back((textureRoot / R"(guns\inactive\shotgun.png)").string().c_str()); //ShotgunInactive
        weaponSprites.emplace_back((textureRoot / R"(guns\inactive\artemis.png)").string().c_str()); //ArtemisInactive
        weaponSprites.emplace_back((textureRoot / R"(guns\inactive\spiral.png)").string().c_str()); //SpiralInactive
        weaponSprites.emplace_back((textureRoot / R"(guns\inactive\kalina.png)").string().c_str()); //KalinaInactive

        weaponSprites.emplace_back((textureRoot / R"(melee\inactive\yamato.png)").string().c_str()); //YamatoInactive
        weaponSprites.emplace_back((textureRoot / R"(melee\inactive\forceedge-vergil.png)").string().c_str()); //YamatoForceEdgeInactive
        
    }
    
    constexpr WeaponTextureID GetWeaponTextureID(WeaponIDs id, bool activeState)
    {
        switch (id)
        {
        case WeaponIDs::RebellionDormant:
            return activeState ? WeaponTextureID::RebellionDormantActive : WeaponTextureID::RebellionDormantInactive;
    
        case WeaponIDs::RebellionAwakened:
            return activeState ? WeaponTextureID::RebellionAwakenedActive : WeaponTextureID::RebellionAwakenedInactive;
    
        case WeaponIDs::ForceEdge:
            return activeState ? WeaponTextureID::ForceEdgeActive : WeaponTextureID::ForceEdgeInactive;
    
        case WeaponIDs::Cerberus:
            return activeState ? WeaponTextureID::CerberusActive : WeaponTextureID::CerberusInactive;
    
        case WeaponIDs::AgniRudra:
            return activeState ? WeaponTextureID::AgniRudraActive : WeaponTextureID::AgniRudraInactive;
    
        case WeaponIDs::Nevan:
            return activeState ? WeaponTextureID::NevanActive : WeaponTextureID::NevanInactive;
    
        case WeaponIDs::Beowulf:
            return activeState ? WeaponTextureID::BeowulfActive : WeaponTextureID::BeowulfInactive;
    
        case WeaponIDs::Yamato:
            return activeState ? WeaponTextureID::YamatoActive : WeaponTextureID::YamatoInactive;
    
        case WeaponIDs::ForceEdgeYamato:
            return activeState ? WeaponTextureID::YamatoForceEdgeActive : WeaponTextureID::YamatoForceEdgeInactive;
    
        case WeaponIDs::EbonyIvory:
            return activeState ? WeaponTextureID::EbonyIvoryActive : WeaponTextureID::EbonyIvoryInactive;
    
        case WeaponIDs::Shotgun:
            return activeState ? WeaponTextureID::ShotgunActive : WeaponTextureID::ShotgunInactive;
    
        case WeaponIDs::Artemis:
            return activeState ? WeaponTextureID::ArtemisActive : WeaponTextureID::ArtemisInactive;
    
        case WeaponIDs::Spiral:
            return activeState ? WeaponTextureID::SpiralActive : WeaponTextureID::SpiralInactive;
    
        case WeaponIDs::KalinaAnn:
            return activeState ? WeaponTextureID::KalinaActive : WeaponTextureID::KalinaInactive;
    
        default:
            return WeaponTextureID::Size;
        }
    
        return WeaponTextureID::Size;
    }

    constexpr TextureID GetWeaponSlotTextureID(size_t slot, size_t charIdx, WeaponState state) {
        size_t baseSlotIndex = (size_t)TextureID::Char1Slot1Inactive + (3 * slot) + (15 * charIdx);

        switch (state)             
        {
        case WeaponState::Inactive:
            return (TextureID)baseSlotIndex;

        case WeaponState::Active:
            return (TextureID)(baseSlotIndex + 1);

        case WeaponState::Duplicate:
            return (TextureID)(baseSlotIndex + 2);
        }
    }
    
    constexpr WeaponTextureID GetDupAnimationWeaponTextureID(WeaponIDs id)
    {
        switch (id)
        {
        case WeaponIDs::RebellionDormant:
            return WeaponTextureID::DuplicateRebellionDormantActive;
    
        case WeaponIDs::RebellionAwakened:
            return WeaponTextureID::DuplicateRebellionAwakenedActive;
    
        case WeaponIDs::ForceEdge:
            return WeaponTextureID::DuplicateForceEdgeActive;
    
        case WeaponIDs::Cerberus:
            return WeaponTextureID::DuplicateCerberusActive;
    
        case WeaponIDs::AgniRudra:
            return WeaponTextureID::DuplicateAgniRudraActive;
    
        case WeaponIDs::Nevan:
            return WeaponTextureID::DuplicateNevanActive;
    
        case WeaponIDs::Beowulf:
            return WeaponTextureID::DuplicateBeowulfActive;
    
        case WeaponIDs::Yamato:
            return WeaponTextureID::DuplicateYamatoActive;
    
        case WeaponIDs::ForceEdgeYamato:
            return WeaponTextureID::DuplicateYamatoForceEdgeActive;
    
        case WeaponIDs::EbonyIvory:
            return WeaponTextureID::DuplicateEbonyIvoryActive;
    
        case WeaponIDs::Shotgun:
            return WeaponTextureID::DuplicateShotgunActive;
    
        case WeaponIDs::Artemis:
            return WeaponTextureID::DuplicateArtemisActive;
    
        case WeaponIDs::Spiral:
            return WeaponTextureID::DuplicateSpiralActive;
    
        case WeaponIDs::KalinaAnn:
            return WeaponTextureID::DuplicateKalinaActive;
    
        default:
            return WeaponTextureID::Size;
        }
    
        return WeaponTextureID::Size;
    }

	void DefineWeaponSprites(std::vector<WeaponIDs>(&weapons)[CHARACTER_COUNT], std::vector <Graphics::Sprite>& sprites, 
        std::vector <Graphics::Sprite>&weaponSprites) {

        size_t baseSlot = 0;

        for (size_t charIdx = 0; charIdx < CHARACTER_COUNT; charIdx++) {
			for (size_t i = 0; i < 5; i++) {
				if (i >= weapons[charIdx].size()) {
					// If out of bounds of the weapon loadout, we still fill it with the weapon on slot 1
					sprites.emplace_back(weaponSprites[(size_t)GetWeaponTextureID(weapons[0][charIdx], false)]);  // Slot i Inactive Texture
					sprites.emplace_back(weaponSprites[(size_t)GetWeaponTextureID(weapons[0][charIdx], true)]);   // Slot i Active Texture
					sprites.emplace_back(weaponSprites[(size_t)GetDupAnimationWeaponTextureID(weapons[0][charIdx])]);       // Slot i Duplicate Texture
				}
				else {
					// Filling it normally
					sprites.emplace_back(weaponSprites[(size_t)GetWeaponTextureID(weapons[charIdx][i], false)]);  // Slot i Inactive Texture
					sprites.emplace_back(weaponSprites[(size_t)GetWeaponTextureID(weapons[charIdx][i], true)]);   // Slot i Active Texture
					sprites.emplace_back(weaponSprites[(size_t)GetDupAnimationWeaponTextureID(weapons[charIdx][i])]);       // Slot i Duplicate Texture
				}
			}
        }
	}

    void RedefineWeaponSprites(std::vector<WeaponIDs>(&weapons)[CHARACTER_COUNT], std::vector <Graphics::Sprite>& sprites,
        std::vector <Graphics::Sprite>&weaponSprites) {

        size_t texID = (size_t)TextureID::Char1Slot1Inactive;
        size_t texIDLast = (size_t)TextureID::Size;
        size_t baseSlot = 0;

        // Erase all weaponSprites from m_Sprites
		sprites.erase(sprites.begin() + texID, sprites.begin() + texIDLast);
		
        DefineWeaponSprites(weapons, sprites, weaponSprites);
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
    
    constexpr std::vector<size_t> GetSpriteIDs(WheelThemes themeID, const std::vector<WeaponIDs>(&weaponIDs)[CHARACTER_COUNT], size_t currentCharIdx)
    {
        std::vector<size_t> spriteIds;
    
        spriteIds.reserve(weaponIDs[currentCharIdx].size() * 6);
    
        // Order matters here, back to front
        {
            spriteIds.push_back((size_t)GetCenterTextureID(themeID)); // 1st: The center piece
    
            for (size_t i = 0; i < weaponIDs[currentCharIdx].size(); i++)
            {
                if (i > 4)
                    break;
    
                spriteIds.push_back((size_t)GetArrowTextureID(themeID, i)); // 2nd: The arrows
            }
    
            for (size_t i = 0; i < weaponIDs[currentCharIdx].size(); i++)
            {
                if (i > 4)
                    break;
    
                spriteIds.push_back((size_t)GetPanelTextureID(themeID, i, false)); // 3rd: The inactive panels
            }
    
            for (size_t i = 0; i < weaponIDs[currentCharIdx].size(); i++)
            {
                if (i > 4)
                    break;
    
                spriteIds.push_back((size_t)GetPanelTextureID(themeID, i, false)); // 3rd: The inactive panels
            }
    
            for (size_t i = 0; i < weaponIDs[currentCharIdx].size(); i++)
            {
                if (i > 4)
                    break;
    
                spriteIds.push_back((size_t)GetPanelTextureID(themeID, i, true)); // 4th: The active panels
            }

            for (size_t charIdx = 0; charIdx < CHARACTER_COUNT; charIdx++) {
                for (size_t i = 0; i < weaponIDs[charIdx].size(); i++) {
                    if (i > 4)
                        break;

                    spriteIds.push_back((size_t)GetWeaponSlotTextureID(i, charIdx, WeaponState::Inactive)); // 5th: The weapons per slot
                    spriteIds.push_back((size_t)GetWeaponSlotTextureID(i, charIdx, WeaponState::Active));
                    spriteIds.push_back((size_t)GetWeaponSlotTextureID(i, charIdx, WeaponState::Duplicate));

                }
            }

        }
    
        return spriteIds;
    }

	constexpr std::vector<size_t> GetWeaponSpriteIDs(const std::vector<WeaponIDs>& weaponIDs) {
		std::vector<size_t> spriteIds;

		spriteIds.reserve(weaponIDs.size());

		// Order matters here, back to front
		{
			for (size_t i = 0; i < weaponIDs.size(); i++) {
				if (i > 4)
					break;

				spriteIds.push_back((size_t)GetWeaponTextureID(weaponIDs[i], false)); // 5th: The inactive weapons
			}

			for (size_t i = 0; i < weaponIDs.size(); i++) {
				if (i > 4)
					break;

				spriteIds.push_back((size_t)GetWeaponTextureID(weaponIDs[i], true)); // 6th: The active weapons
			}

			for (size_t i = 0; i < weaponIDs.size(); i++) {
				if (i > 4)
					break;

				spriteIds.push_back((size_t)GetDupAnimationWeaponTextureID(weaponIDs[i])); // 7th: The active weapon animation duplicates
			}
		}

		return spriteIds;
	}

    WeaponWheel::WeaponWheel(ID3D11Device* pD3D11Device, ID3D11DeviceContext* pD3D11DeviceContext, UINT width, UINT height,
        std::vector<WeaponIDs> weapons[CHARACTER_COUNT], WheelThemes themeID, bool buttonHeld)
        : m_pD3D11Device(pD3D11Device), m_pD3D11DeviceContext(pD3D11DeviceContext), m_Width(width), m_Height(height),
        m_ThemeID(themeID), m_buttonHeld(buttonHeld)
    {
		// Manually copy each vector in the array
		for (size_t i = 0; i < CHARACTER_COUNT; ++i) {
			m_Weapons[i] = weapons[i];
		}

        if (m_Sprites.size() == 0) {
            LoadSprites(m_Sprites, m_WeaponSprites);
            DefineWeaponSprites(m_Weapons, m_Sprites, m_WeaponSprites);
        }

        const auto spriteIDs = GetSpriteIDs(m_ThemeID, m_Weapons, m_CurrentActiveCharIndex);

        m_pSpriteBatch = std::make_unique<Graphics::BatchedSprites>(m_pD3D11Device, m_Width, m_Height, m_Sprites, spriteIDs);

        for (size_t charIdx = 0; charIdx < CHARACTER_COUNT; charIdx++) {
			for (size_t i = 0; i < m_Weapons[charIdx].size(); i++) {
				m_pSpriteBatch->SetTransform(
					(size_t)GetWeaponSlotTextureID(i, charIdx, WeaponState::Inactive),
					s_MeleeInactiveSlotTransforms[i].Translation,
					s_MeleeInactiveSlotTransforms[i].Rotation,
					s_MeleeInactiveSlotTransforms[i].Scale
				);

				m_pSpriteBatch->SetTransform(
					(size_t)GetWeaponSlotTextureID(i, charIdx, WeaponState::Active),
					s_MeleeActiveSlotTransforms[i].Translation,
					s_MeleeActiveSlotTransforms[i].Rotation,
					s_MeleeActiveSlotTransforms[i].Scale
				);

				m_pSpriteBatch->SetTransform(
					(size_t)GetWeaponSlotTextureID(i, charIdx, WeaponState::Duplicate),
					s_MeleeActiveSlotTransforms[i].Translation,
					s_MeleeActiveSlotTransforms[i].Rotation,
					s_MeleeActiveSlotTransforms[i].Scale
				);


				m_pSpriteBatch->SetOpacity(
					(size_t)GetWeaponSlotTextureID(i, charIdx, WeaponState::Duplicate),
					0.0f
				);
			}
        }
		
        InitializeAnimations();
    }

    WeaponWheel::~WeaponWheel()
    {}

    void WeaponWheel::ReloadWheel(std::vector<WeaponIDs> weapons[CHARACTER_COUNT])
    {
		// Manually copy each vector in the array
		for (size_t i = 0; i < CHARACTER_COUNT; ++i) {
			m_Weapons[i] = weapons[i];
		}

        RedefineWeaponSprites(m_Weapons, m_Sprites, m_WeaponSprites);
        const auto spriteIDs = GetSpriteIDs(m_ThemeID, m_Weapons, m_CurrentActiveCharIndex);

        
		m_pSpriteBatch = std::make_unique<Graphics::BatchedSprites>(m_pD3D11Device, m_Width, m_Height, m_Sprites, spriteIDs);
        //m_pSpriteBatch->SetActiveSprites(spriteIDs);

		for (size_t charIdx = 0; charIdx < CHARACTER_COUNT; charIdx++) {
			for (size_t i = 0; i < m_Weapons[charIdx].size(); i++) {
				m_pSpriteBatch->SetTransform(
					(size_t)GetWeaponSlotTextureID(i, charIdx, WeaponState::Inactive),
					s_MeleeInactiveSlotTransforms[i].Translation,
					s_MeleeInactiveSlotTransforms[i].Rotation,
					s_MeleeInactiveSlotTransforms[i].Scale
				);

				m_pSpriteBatch->SetTransform(
					(size_t)GetWeaponSlotTextureID(i, charIdx, WeaponState::Active),
					s_MeleeActiveSlotTransforms[i].Translation,
					s_MeleeActiveSlotTransforms[i].Rotation,
					s_MeleeActiveSlotTransforms[i].Scale
				);

				m_pSpriteBatch->SetTransform(
					(size_t)GetWeaponSlotTextureID(i, charIdx, WeaponState::Duplicate),
					s_MeleeActiveSlotTransforms[i].Translation,
					s_MeleeActiveSlotTransforms[i].Rotation,
					s_MeleeActiveSlotTransforms[i].Scale
				);


				m_pSpriteBatch->SetOpacity(
					(size_t)GetWeaponSlotTextureID(i, charIdx, WeaponState::Duplicate),
					0.0f
				);
			}
		}

        UpdateSlotStates();
    }

    void WeaponWheel::UpdateWeapons(std::vector<WeaponIDs> weapons[CHARACTER_COUNT]) {
		// Manually copy each vector in the array
		for (size_t i = 0; i < CHARACTER_COUNT; ++i) {
			m_Weapons[i] = weapons[i];
		}

        const auto spriteIDs = GetSpriteIDs(m_ThemeID, m_Weapons, m_CurrentActiveCharIndex);
        m_pSpriteBatch->SetActiveSprites(spriteIDs);

        UpdateSlotStates();
    }

    void WeaponWheel::UpdateCharIndex(size_t newCharIdx) {
        m_CurrentActiveCharIndex = newCharIdx;
    }

    void WeaponWheel::SetWheelTheme(WheelThemes themeID)
    {
        m_ThemeID = themeID;

        const auto spriteIDs = GetSpriteIDs(m_ThemeID, m_Weapons, m_CurrentActiveCharIndex);

        m_pSpriteBatch->SetActiveSprites(spriteIDs);
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
        if (!m_buttonHeld)
            m_SinceLatestChangeMsGlobal += tsGlobal;
        else
            m_SinceLatestChangeMsGlobal = 0;
    }

    void WeaponWheel::TrackButtonHeldState(bool buttonHeld) 
    {
        m_buttonHeld = buttonHeld;
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

                    for (size_t i = 0; i < m_Weapons[m_CurrentActiveCharIndex].size(); i++)
                    {
                        // Inactive slots opacity
                        if (i != m_CurrentActiveSlot)
                        {
                            m_pSpriteBatch->SetOpacity((size_t)GetWeaponSlotTextureID(i, m_CurrentActiveCharIndex, WeaponState::Inactive), 1.0f - progress);
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
                    m_pSpriteBatch->SetOpacity((size_t)GetWeaponSlotTextureID(m_CurrentActiveSlot, m_CurrentActiveCharIndex, WeaponState::Active), 1.0f);
                });

            // On update
            m_pActiveWeaponFadeAnimation->SetOnUpdate([this](GenericAnimation* pAnim)
                {
                    const auto progress = pAnim->GetProgressNormalized();

                    m_pSpriteBatch->SetOpacity((size_t)GetWeaponSlotTextureID(m_CurrentActiveSlot, m_CurrentActiveCharIndex, WeaponState::Active), 1.0f - progress);
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
                    m_pSpriteBatch->SetBrightness((size_t)GetWeaponSlotTextureID(m_CurrentActiveSlot, m_CurrentActiveCharIndex, WeaponState::Active), 1.0f);
                });

            // On update
            m_pWeaponSwitchBrightnessAnimation->SetOnUpdate([this](GenericAnimation* pAnim)
                {
                    const auto progress = pAnim->GetProgressNormalized();

                    m_pSpriteBatch->SetBrightness((size_t)GetWeaponSlotTextureID(m_CurrentActiveSlot, m_CurrentActiveCharIndex, WeaponState::Active), progress * 3.18f);
                });

            // After the animation ends
            m_pWeaponSwitchBrightnessAnimation->SetOnEnd([this](GenericAnimation* pAnim)
                {
                    m_RunWeaponSwitchBrightnessAnim = false;
                    m_pSpriteBatch->SetBrightness((size_t)GetWeaponSlotTextureID(m_CurrentActiveSlot, m_CurrentActiveCharIndex, WeaponState::Active), 1.0f);
                });

            // When reset
            m_pWeaponSwitchBrightnessAnimation->SetOnReset([this](GenericAnimation* pAnim)
                {
                    m_RunWeaponSwitchBrightnessAnim = false;
                    m_pSpriteBatch->SetBrightness((size_t)GetWeaponSlotTextureID(m_CurrentActiveSlot, m_CurrentActiveCharIndex, WeaponState::Active), 1.0f);
                });
        }

        {
            m_pWeaponSwitchScaleAnimation = std::make_unique<GenericAnimation>(s_SwitchScaleAnimDur);

            // Before the animation starts
            m_pWeaponSwitchScaleAnimation->SetOnStart([this](GenericAnimation* pAnim)
                {
                    m_pSpriteBatch->SetOpacity(
                        (size_t)GetWeaponSlotTextureID(m_CurrentActiveSlot, m_CurrentActiveCharIndex, WeaponState::Duplicate), 0.0f);

                    m_pSpriteBatch->SetBrightness(
                        (size_t)GetWeaponSlotTextureID(m_CurrentActiveSlot, m_CurrentActiveCharIndex, WeaponState::Duplicate), 1.0f);

                    m_pSpriteBatch->ScaleTo(
                        (size_t)GetWeaponSlotTextureID(m_CurrentActiveSlot, m_CurrentActiveCharIndex, WeaponState::Duplicate), glm::vec3(1.0f));
                });

            // On update
            m_pWeaponSwitchScaleAnimation->SetOnUpdate([this](GenericAnimation* pAnim)
                {
                    const auto progress = pAnim->GetProgressNormalized();

                    m_pSpriteBatch->SetOpacity(
                        (size_t)GetWeaponSlotTextureID(m_CurrentActiveSlot, m_CurrentActiveCharIndex, WeaponState::Duplicate), progress * 0.9f);

                    m_pSpriteBatch->SetBrightness(
                       (size_t)GetWeaponSlotTextureID(m_CurrentActiveSlot, m_CurrentActiveCharIndex, WeaponState::Duplicate), progress * 3.18f);

                    m_pSpriteBatch->ScaleTo(
                        (size_t)GetWeaponSlotTextureID(m_CurrentActiveSlot, m_CurrentActiveCharIndex, WeaponState::Duplicate),
                        s_MeleeActiveSlotTransforms[m_CurrentActiveSlot].Scale * float(progress) * 1.5f);
                });

            // After the animation ends
            m_pWeaponSwitchScaleAnimation->SetOnEnd([this](GenericAnimation* pAnim)
                {
                    m_RunWeaponSwitchScaleAnim = false;

                    m_pSpriteBatch->SetOpacity(
                        (size_t)GetWeaponSlotTextureID(m_CurrentActiveSlot, m_CurrentActiveCharIndex, WeaponState::Duplicate), 0.0f);

                    m_pSpriteBatch->SetBrightness(
                        (size_t)GetWeaponSlotTextureID(m_CurrentActiveSlot, m_CurrentActiveCharIndex, WeaponState::Duplicate), 1.0f);

                    m_pSpriteBatch->ScaleTo(
                        (size_t)GetWeaponSlotTextureID(m_CurrentActiveSlot, m_CurrentActiveCharIndex, WeaponState::Duplicate), glm::vec3(1.0f));
                });

            // When reset
            m_pWeaponSwitchScaleAnimation->SetOnReset([this](GenericAnimation* pAnim)
                {
                    m_RunWeaponSwitchScaleAnim = false;

                    m_pSpriteBatch->SetOpacity(
                        (size_t)GetWeaponSlotTextureID(m_CurrentActiveSlot, m_CurrentActiveCharIndex, WeaponState::Duplicate), 0.0f);

                    m_pSpriteBatch->SetBrightness(
                        (size_t)GetWeaponSlotTextureID(m_CurrentActiveSlot, m_CurrentActiveCharIndex, WeaponState::Duplicate), 1.0f);

                    m_pSpriteBatch->ScaleTo(
                        (size_t)GetWeaponSlotTextureID(m_CurrentActiveSlot, m_CurrentActiveCharIndex, WeaponState::Duplicate), glm::vec3(1.0f));
                });
        }
    }

    void WeaponWheel::SetInactiveOpacity(float opacity)
    {
        for (size_t i = 0; i < m_Weapons[m_CurrentActiveCharIndex].size(); i++)
        {
            m_pSpriteBatch->SetOpacity((size_t)GetWeaponSlotTextureID(i, m_CurrentActiveCharIndex, WeaponState::Inactive), opacity);
        }
    }

    void WeaponWheel::SetActiveWeaponOpacity(float opacity)
    {
        if (m_CurrentActiveSlot > 4) // 5 slots is max
            return;

        m_pSpriteBatch->SetOpacity((size_t)GetWeaponSlotTextureID(m_CurrentActiveSlot, m_CurrentActiveCharIndex, WeaponState::Active), opacity);
    }

    void WeaponWheel::UpdateSlotStates()
    {
        if (m_CurrentActiveSlot > 4)
        {
            for (size_t i = 0; i < m_Weapons[m_CurrentActiveCharIndex].size(); i++)
            {
                m_pSpriteBatch->SetOpacity((size_t)GetWeaponSlotTextureID(i, m_CurrentActiveCharIndex, WeaponState::Inactive), 1.0f);
                m_pSpriteBatch->SetOpacity((size_t)GetWeaponSlotTextureID(i, m_CurrentActiveCharIndex, WeaponState::Active), 0.0f);
                m_pSpriteBatch->SetOpacity((size_t)GetPanelTextureID(m_ThemeID, i, false), 1.0f);
                m_pSpriteBatch->SetOpacity((size_t)GetPanelTextureID(m_ThemeID, i, true), 0.0f);
                m_pSpriteBatch->SetOpacity((size_t)GetArrowTextureID(m_ThemeID, i), 1.0f);
            }

            m_pSpriteBatch->SetOpacity((size_t)GetCenterTextureID(m_ThemeID), 1.0f);

            return;
        }

        // Set all inactive chars' weapons to be invisible
        for (size_t charIdx = 0; charIdx < CHARACTER_COUNT; charIdx++) {
            if (charIdx != m_CurrentActiveCharIndex) {
                for (size_t i = 0; i < m_Weapons[charIdx].size(); i++) {
                    m_pSpriteBatch->SetOpacity((size_t)GetWeaponSlotTextureID(i, charIdx, WeaponState::Inactive), 0.0f);
                    m_pSpriteBatch->SetOpacity((size_t)GetWeaponSlotTextureID(i, charIdx, WeaponState::Active), 0.0f);
                }
            }
        }

        for (size_t i = 0; i < m_Weapons[m_CurrentActiveCharIndex].size(); i++)
        {
            m_pSpriteBatch->SetOpacity((size_t)GetWeaponSlotTextureID(i, m_CurrentActiveCharIndex, WeaponState::Inactive), 1.0f);
            m_pSpriteBatch->SetOpacity((size_t)GetWeaponSlotTextureID(i, m_CurrentActiveCharIndex, WeaponState::Active), 0.0f);
            m_pSpriteBatch->SetOpacity((size_t)GetPanelTextureID(m_ThemeID, i, false), 1.0f);
            m_pSpriteBatch->SetOpacity((size_t)GetPanelTextureID(m_ThemeID, i, true), 0.0f);
            m_pSpriteBatch->SetOpacity((size_t)GetArrowTextureID(m_ThemeID, i), 0.0f);
        }

        m_pSpriteBatch->SetOpacity((size_t)GetCenterTextureID(m_ThemeID), 1.0f);

        m_pSpriteBatch->SetOpacity((size_t)GetWeaponSlotTextureID(m_CurrentActiveSlot, m_CurrentActiveCharIndex, WeaponState::Inactive), 0.0f);
        m_pSpriteBatch->SetOpacity((size_t)GetWeaponSlotTextureID(m_CurrentActiveSlot, m_CurrentActiveCharIndex, WeaponState::Active), 1.0f);
        m_pSpriteBatch->SetOpacity((size_t)GetPanelTextureID(m_ThemeID, m_CurrentActiveSlot, false), 0.0f);
        m_pSpriteBatch->SetOpacity((size_t)GetPanelTextureID(m_ThemeID, m_CurrentActiveSlot, true), 1.0f);
        m_pSpriteBatch->SetOpacity((size_t)GetArrowTextureID(m_ThemeID, m_CurrentActiveSlot), 1.0f);
    }
}