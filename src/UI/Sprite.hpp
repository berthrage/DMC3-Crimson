#pragma once

#include "GraphicsBase.hpp"

namespace Graphics
{
	class SpriteDesc : public Drawable
	{
	public:
		SpriteDesc(std::string texturePath, Rect bb = { -1.0f, 1.0f, 1.0f, -1.0f },
			Rect uv = { 0.0f, 0.0f, 1.0f, 1.0f });
		virtual ~SpriteDesc();

		const auto& GetTexturePath() const { return m_TexturePath; };

	private:
		std::string m_TexturePath;
	};
}
