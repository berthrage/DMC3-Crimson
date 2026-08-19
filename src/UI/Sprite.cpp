#include "Sprite.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtx\euler_angles.hpp"

namespace Graphics
{
    SpriteDesc::SpriteDesc(const std::string& texturePath, size_t arrayIdx, Rect regionRect,
                           Color tint, float opacity, float brightness,
                           glm::vec3 translation, glm::vec3 rotationEuler, glm::vec3 scale)
        : m_TexturePath(texturePath), m_ArrayIdx(arrayIdx), m_RegionRect(regionRect), m_Tint(tint),
          m_Opacity(opacity), m_Brightness(brightness),
          m_Translation(translation), m_RotationEuler(rotationEuler), m_Scale(scale)
    {}

    SpriteDesc::~SpriteDesc()
    {}

    glm::mat4x4 SpriteDesc::GetTransformationMatrix()
    {
        // Convert Euler to quaternion to avoid gimbal lock if needed
        glm::quat rotationQuat = glm::quat(m_RotationEuler); // glm default: pitch, yaw, roll

        // Compose the transform: Translation * Rotation * Scale
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Translation) *
            glm::mat4_cast(rotationQuat) *
            glm::scale(glm::mat4(1.0f), m_Scale);

        return transform;
    }
}
