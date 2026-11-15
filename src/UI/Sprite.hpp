#pragma once

#include "GraphicsBase.hpp"

namespace Graphics
{
	class SpriteDesc
	{
	public:
		SpriteDesc(const std::string& texturePath, size_t arrayIdx = 0,
				   Rect regionRect = { 0.0f, 0.0f, 1.0f, 1.0f }, Color tint = { 1.0f, 1.0f, 1.0f, 1.0f },
				   float opacity = 1.0f, float brightness = 1.0f,
				   glm::vec3 translation = { 0.0f, 0.0f, 0.0f }, glm::vec3 rotationEuler = { 0.0f, 0.0f, 0.0f },
				   glm::vec3 scale = { 1.0f, 1.0f, 1.0f });
		virtual ~SpriteDesc();

		const auto& GetTexturePath() const { return m_TexturePath; }
		const auto& GetArrayIdx() const { return m_ArrayIdx; };
		const auto& GetRegionRect() const { return m_RegionRect; }
		const auto& GetTint() const { return m_Tint; }
		const auto& GetOpacity() const { return m_Opacity; }
		const auto& GetBrightness() const { return m_Brightness; }
		const auto& GetTranslation() const { return m_Translation; }
		const auto& GetScale() const { return m_Scale; }
		const auto& GetRotation() const { return m_RotationEuler; }

		const auto GetRegionRectAspectRatio() const { return (m_RegionRect.R - m_RegionRect.L) / (m_RegionRect.B - m_RegionRect.T); }

		void SetTextureIdx(size_t arrayIdx) { m_ArrayIdx = arrayIdx; }
		void SetRegionRect(Rect regionRect) { m_RegionRect = regionRect; } // Normalized
		void SetTint(Color tint) { m_Tint = tint; } // Normalized
		void SetOpacity(float opacity) { m_Opacity = opacity; }
		void SetBrightness(float brightness) { m_Brightness = brightness; }
		void SetTranslation(glm::vec3 translation) { m_Translation = translation; } // Normalized
		void SetScale(glm::vec3 scale) { m_Scale = scale;  } // Normalized
		void SetRotation(glm::vec3 rotationEuler) { m_RotationEuler = rotationEuler; } // Normalized

		glm::mat4x4 GetTransformationMatrix();

	private:
		std::string m_TexturePath;
		size_t m_ArrayIdx;
		Rect m_RegionRect;
		Color m_Tint;
		float m_Opacity, m_Brightness;
		glm::vec3 m_Translation, m_RotationEuler, m_Scale;
	};
}
