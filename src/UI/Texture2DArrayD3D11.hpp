#pragma once

#include "GraphicsBase.hpp"

namespace Graphics
{
	class Texture2DArrayD3D11 : public ErrorHandled
	{
	public:
		Texture2DArrayD3D11() = default;

		Texture2DArrayD3D11(const std::vector<std::string>& filePaths, ID3D11Device* pD3D11Device);

		ID3D11ShaderResourceView* GetSrv() const { return m_SRV.Get(); }
		int GetWidth() const { return m_Width; }
		int GetHeight() const { return m_Height; }

		const auto& GetCount() const { return m_Count; }

		glm::u64vec2 GetSize() const { return glm::u64vec2(m_Width, m_Height); }

		template<typename T = float>
		glm::u64vec2 GetSize(const T scale) const { return Vec2<T>(m_Width, m_Height) * scale; }
		template<typename T = float>
		glm::u64vec2 GetSizeByWRatio(const T nWidth) const { return glm::u64vec2(m_Width, m_Height) * (nWidth / m_Width); }
		template<typename T = float>
		glm::u64vec2 GetSizeByHRatio(const T nHeight) const { return glm::u64vec2(m_Width, m_Height) * (nHeight / m_Height); }
		bool IsValid() const { return m_IsLoaded; }

		inline operator void* () { return GetSrv(); }
		inline operator void* () const { return GetSrv(); }
		inline operator ID3D11ShaderResourceView* () { return GetSrv(); }
		inline operator ID3D11ShaderResourceView* () const { return GetSrv(); }
		inline operator bool() { return IsValid(); }
		inline operator bool() const { return IsValid(); }

	private:
		bool Commit(ID3D11Device* pD3D11Device);

	private:
		std::vector<unsigned char*> m_ImageDataPtrs{};

		ComPtr<ID3D11ShaderResourceView> m_SRV{};
		int m_Width{ 0 };
		int m_Height{ 0 };
		size_t m_Count{ 0 };

		bool m_IsLoaded{ false };
	};
}