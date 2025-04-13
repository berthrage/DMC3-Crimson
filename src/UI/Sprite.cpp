#include "Sprite.hpp"

namespace Graphics
{
    Sprite::Sprite(std::string texturePath, Rect bb, Rect uv)
        : m_TexturePath(texturePath),
        Drawable(
            // Quad
            {
                { { bb.L, bb.T, 0.0f }, { uv.L, uv.T } , { 1.0f, 1.0f, 1.0f, 1.0f } }, // Top left
                { { bb.R, bb.B, 0.0f }, { uv.R, uv.B } , { 1.0f, 1.0f, 1.0f, 1.0f } }, // Bot right
                { { bb.L, bb.B, 0.0f }, { uv.L, uv.B } , { 1.0f, 1.0f, 1.0f, 1.0f } }, // Bot left
                { { bb.R, bb.T, 0.0f }, { uv.R, uv.T } , { 1.0f, 1.0f, 1.0f, 1.0f } }, // Top right
            },
            { 0, 1, 2, 0, 3, 1 }
            )
    {}

    Sprite::~Sprite()
    {}
}
