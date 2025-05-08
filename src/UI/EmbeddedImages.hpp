#pragma once

#include <inttypes.h>
#include <tuple>

#include "..\..\ThirdParty\stb\stb_image.h"
#include "..\..\ThirdParty\stb\stb_image_resize2.h"

unsigned char* STBResize(const unsigned char* inputPixels, int inputW, int inputH, int inputStrideInBytes,
	unsigned char* outputPixels, int outputW, int outputH, int outputStrideInBytes,
	int numChannels);
unsigned int STBDecompressLength(const unsigned char* input);
void STBMatch(const unsigned char* data, unsigned int length);
void STBLit(const unsigned char* data, unsigned int length);
const unsigned char* STBDecompressToken(const unsigned char* i);
unsigned int SRBAdler32(unsigned int adler32, unsigned char* buffer, unsigned int buflen);
unsigned int STBDecompress(unsigned char* output, const unsigned char* i, unsigned int length);

class Image {
public:
	Image(const uint32_t width, const uint32_t height,
		const uint32_t compressedSize, const uint8_t* compressedData);

	~Image();

	void Resize(uint32_t newWidth, uint32_t newHeight);

	void ResizeByRatioH(uint32_t newHeight);

	void ResizeByRatioW(uint32_t newWidth);

	auto GetWidth() const { return m_Width; }
	auto GetHeight() const { return m_Height; };
	auto GetRGBAData() const { return m_RGBAData; }

private:
	uint32_t		m_Width{ 0 };
	uint32_t		m_Height{ 0 };
	uint8_t*		m_RGBAData{ nullptr };
	uint32_t		m_IimageCompressedSize{ 0 };
	const uint8_t*	m_image_compressed_data{ nullptr };
};

// 1598 x 317
class CrimsonHeaderLogo : public Image {
public:
	CrimsonHeaderLogo()
		: Image(m_CompressedWidth, m_CompressedHeight,
			m_ImageCompressedSize, reinterpret_cast<const uint8_t*>(m_ImageCompressedData))
	{}

protected:
	static constexpr uint32_t	m_CompressedWidth{ 1598 };
	static constexpr uint32_t	m_CompressedHeight{ 317 };
	static constexpr uint32_t	m_ImageCompressedSize{ 493597 };
	static			 uint32_t	m_ImageCompressedData[493600 / 4];
};

extern CrimsonHeaderLogo g_Image_CrimsonHeaderLogo;

// 1924 x 509
class CrimsonMainLogo : public Image {
public:
	CrimsonMainLogo()
		: Image(m_CompressedWidth, m_CompressedHeight,
			m_ImageCompressedSize, reinterpret_cast<const uint8_t*>(m_ImageCompressedData))
	{}

protected:
	static constexpr uint32_t	m_CompressedWidth{ 1924 };
	static constexpr uint32_t	m_CompressedHeight{ 509 };
	static constexpr uint32_t	m_ImageCompressedSize{ 851506 };
	static			 uint32_t	m_ImageCompressedData[851508 / 4];
};

extern CrimsonMainLogo g_Image_CrimsonMainLogo;

// 1177 x 355
class VanillaLogo : public Image {
public:
	VanillaLogo()
		: Image(m_CompressedWidth, m_CompressedHeight,
			m_ImageCompressedSize, reinterpret_cast<const uint8_t*>(m_ImageCompressedData))
	{}

protected:
	static constexpr uint32_t	m_CompressedWidth{ 1177 };
	static constexpr uint32_t	m_CompressedHeight{ 355 };
	static constexpr uint32_t	m_ImageCompressedSize{ 43258 };
	static			 uint32_t	m_ImageCompressedData[43260 / 4];
};

extern VanillaLogo g_Image_VanillaLogo;

// 875 x 506
class StyleSwitcherLogo : public Image {
public:
	StyleSwitcherLogo()
		: Image(m_CompressedWidth, m_CompressedHeight,
			m_ImageCompressedSize, reinterpret_cast<const uint8_t*>(m_ImageCompressedData))
	{}

protected:
	static constexpr uint32_t	m_CompressedWidth{ 875 };
	static constexpr uint32_t	m_CompressedHeight{ 506 };
	static constexpr uint32_t	m_ImageCompressedSize{ 374318 };
	static			 uint32_t	m_ImageCompressedData[374320 / 4];
};

extern StyleSwitcherLogo g_Image_StyleSwitcherLogo;

// 2048 x 2048
class SocialsIcons : public Image {
public:
	SocialsIcons()
		: Image(m_CompressedWidth, m_CompressedHeight,
			m_ImageCompressedSize, reinterpret_cast<const uint8_t*>(m_ImageCompressedData))
	{}

	enum {
		ID_YouTube = 0,
		ID_Twitter,
		ID_Discord,
		ID_Github,
		ID_Reddit,
		ID_Patreon,
		ID_Nexusmods,
	};

	static constexpr std::tuple<std::tuple<float, float>, std::tuple<float, float>> GetUVRect(const size_t socialID) {
		switch (socialID) {
		case ID_YouTube:
			return { 
				{ 2.0f / m_CompressedWidth, 2.0f / m_CompressedHeight }, 
				{ 442.0f / m_CompressedWidth, 442.0f / m_CompressedHeight } 
			};
			break;
		
		case ID_Twitter:
			return { 
				{ 2.0f / m_CompressedWidth, 446.0f / m_CompressedHeight },
				{ 432.0f / m_CompressedWidth, 876.0f / m_CompressedHeight } 
			};
			break;

		case ID_Discord:
			return { 
				{ 446.0f / m_CompressedWidth, 2.0f / m_CompressedHeight },
				{ 807.0f / m_CompressedWidth, 363.0f / m_CompressedHeight }
			};
			break;

		case ID_Github:
			return { 
				{ 2.0f / m_CompressedWidth, 880.0f / m_CompressedHeight }, 
				{ 354.0f / m_CompressedWidth, 1232.0f / m_CompressedHeight } 
			};
			break;

		case ID_Reddit:
			return { 
				{ 436.0f / m_CompressedWidth, 446.0f / m_CompressedHeight },
				{ 778.0f / m_CompressedWidth, 788.0f / m_CompressedHeight } 
			};
			break;

		case ID_Patreon:
			return {
				{ 811.0f / m_CompressedWidth, 2.0f / m_CompressedHeight },
				{ 1110.0f / m_CompressedWidth, 301.0f / m_CompressedHeight }
			};

		case ID_Nexusmods:
			return { 
				{ 2.0f / m_CompressedWidth, 1236.0f / m_CompressedHeight },
				{ 285.0f / m_CompressedWidth, 1519.0f / m_CompressedHeight } 
			};
			break;
			break;

		default:
			return { { 0.0f, 0.0f }, { 0.0f, 0.0f } };
			break;
		}

		return { { 0.0f, 0.0f }, { 0.0f, 0.0f } };
	}

protected:
	static constexpr uint32_t	m_CompressedWidth{ 2048 };
	static constexpr uint32_t	m_CompressedHeight{ 2048 };
	static constexpr uint32_t	m_ImageCompressedSize{ 95479 };
	static			 uint32_t	m_ImageCompressedData[95480 / 4];
};

extern SocialsIcons g_Image_SocialIcons;


// 142 x 200
class RedOrb : public Image {
public:
	RedOrb()
		: Image(m_CompressedWidth, m_CompressedHeight,
			m_ImageCompressedSize, reinterpret_cast<const uint8_t*>(m_ImageCompressedData)) {
	}

protected:
	static constexpr uint32_t	m_CompressedWidth{ 142 };
	static constexpr uint32_t	m_CompressedHeight{ 200 };
	static constexpr uint32_t m_ImageCompressedSize{ 39475 };
	static			 uint32_t m_ImageCompressedData[39476 / 4];
};

extern RedOrb g_Image_RedOrb;

// 178 x 250
class RedOrbVanilla : public Image {
public:
	RedOrbVanilla()
		: Image(m_CompressedWidth, m_CompressedHeight,
			m_ImageCompressedSize, reinterpret_cast<const uint8_t*>(m_ImageCompressedData)) {
	}

protected:
	static constexpr uint32_t	m_CompressedWidth{ 178 };
	static constexpr uint32_t	m_CompressedHeight{ 250 };
	static constexpr uint32_t m_ImageCompressedSize{ 58259 };
	static			 uint32_t m_ImageCompressedData[58260 / 4];
};

extern RedOrbVanilla g_Image_RedOrbVanilla;

// 178 x 250
class RedOrbStyleSwitcher : public Image {
public:
	RedOrbStyleSwitcher()
		: Image(m_CompressedWidth, m_CompressedHeight,
			m_ImageCompressedSize, reinterpret_cast<const uint8_t*>(m_ImageCompressedData)) {
	}

protected:
	static constexpr uint32_t	m_CompressedWidth{ 178 };
	static constexpr uint32_t	m_CompressedHeight{ 250 };
	static constexpr uint32_t m_ImageCompressedSize{ 70421 };
	static			 uint32_t m_ImageCompressedData[70424 / 4];
};

extern RedOrbStyleSwitcher g_Image_RedOrbStyleSwitcher;

// 178 x 250
class RedOrbCrimson : public Image {
public:
	RedOrbCrimson()
		: Image(m_CompressedWidth, m_CompressedHeight,
			m_ImageCompressedSize, reinterpret_cast<const uint8_t*>(m_ImageCompressedData)) {
	}

protected:
	static constexpr uint32_t	m_CompressedWidth{ 178 };
	static constexpr uint32_t	m_CompressedHeight{ 250 };
	static constexpr uint32_t m_ImageCompressedSize{ 75838 };
	static			 uint32_t m_ImageCompressedData[75840 / 4];
};

extern RedOrbCrimson g_Image_RedOrbCrimson;

// 178 x 250
class RedOrbCustom : public Image {
public:
	RedOrbCustom()
		: Image(m_CompressedWidth, m_CompressedHeight,
			m_ImageCompressedSize, reinterpret_cast<const uint8_t*>(m_ImageCompressedData)) {
	}

protected:
	static constexpr uint32_t	m_CompressedWidth{ 178 };
	static constexpr uint32_t	m_CompressedHeight{ 250 };
	static constexpr uint32_t m_ImageCompressedSize{ 71431 };
	static			uint32_t m_ImageCompressedData[71432 / 4];
};

extern RedOrbCustom g_Image_RedOrbCustom;