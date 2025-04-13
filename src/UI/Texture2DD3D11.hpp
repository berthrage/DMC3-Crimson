#pragma once

#include <d3d11_4.h>
#include <wrl.h>

#include <string>

#include "imgui.h"
#include "imgui_internal.h"

template <typename T>
struct Vec2
{
	Vec2(T _x, T _y)
		: x(_x), y(_y)
	{}

	Vec2(const Vec2& other)
		: x(other.x), y(other.y)
	{}

	Vec2(const Vec2&& other)
		: x(std::move(other.x)), y(std::move(other.y))
	{}

	T x;
	T y;

	// Depends on ImGui imgui_internal.h
	Vec2(const ImVec2& imVec)
		: x(imVec.x), y(imVec.y)
	{}

	operator ImVec2() { return ImVec2(static_cast<float>(x), static_cast<float>(y)); }
	operator ImVec2() const { return ImVec2(static_cast<float>(x), static_cast<float>(y)); }

	Vec2 operator*(const T rhs) { return Vec2(x * rhs, y * rhs); }
	Vec2 operator/(const T rhs) { return Vec2(x / rhs, y / rhs); }
	Vec2 operator+(const Vec2& rhs) { return Vec2(x + rhs.x, y + rhs.y); }
	Vec2 operator-(const Vec2& rhs) { return Vec2(x - rhs.x, y - rhs.y); }
	Vec2 operator*(const Vec2& rhs) { return Vec2(x * rhs.x, y * rhs.y); }
	Vec2 operator/(const Vec2& rhs) { return Vec2(x / rhs.x, y / rhs.y); }
	Vec2& operator*=(const T rhs) { return { x *= rhs, y *= rhs }; }
	Vec2& operator/=(const T rhs) { return { x /= rhs, y /= rhs }; }
	Vec2& operator+=(const Vec2& rhs) { return { x += rhs.x, y += rhs.y }; }
	Vec2& operator-=(const Vec2& rhs) { return { x -= rhs.x, y -= rhs.y }; }
	Vec2& operator*=(const Vec2& rhs) { return { x *= rhs.x, y *= rhs.y }; }
	Vec2& operator/=(const Vec2& rhs) { return { x /= rhs.x, y /= rhs.y }; }
	Vec2& operator=(const Vec2& rhs) { this->x = rhs.x; this->y = rhs.y; return *this; }
	bool operator==(const Vec2& rhs) { return x == rhs.x && y == rhs.y; }
	bool operator!=(const Vec2& rhs) { return x != rhs.x || y != rhs.y; }
};

class Texture2DD3D11 
{
public:
	Texture2DD3D11() = default;

	Texture2DD3D11(const char* filename, ID3D11Device* pd3dDevice);
	Texture2DD3D11(const unsigned char* imageData, size_t imageDataSize, ID3D11Device* pd3dDevice);
	Texture2DD3D11(unsigned char* imageData, int width, int height, ID3D11Device* pd3dDevice);

	unsigned char* GetImageData() const { return m_ImageData; }
	ID3D11ShaderResourceView* GetTexture() const { return m_SRV.Get(); }
	int GetWidth() const { return m_Width; }
	int GetHeight() const { return m_Height; }
	const auto& GetLastError() const { return m_LastError; };

	Vec2<size_t> GetSize() const { return Vec2<size_t>(m_Width, m_Height); }

	template<typename T = float>
	Vec2<T> GetSize(const T scale) const { return Vec2<T>(m_Width, m_Height) * scale; }
	template<typename T = float>
	Vec2<T> GetSizeByWRatio(const T nWidth) const { return Vec2<T>(m_Width, m_Height) * (nWidth / m_Width); }
	template<typename T = float>
	Vec2<T> GetSizeByHRatio(const T nHeight) const { return Vec2<T>(m_Width, m_Height) * (nHeight / m_Height); }
	bool IsValid() const { return m_IsLoaded; }

	inline operator void* () { return GetTexture(); }
	inline operator void* () const { return GetTexture(); }
	inline operator ID3D11ShaderResourceView* () { return GetTexture(); }
	inline operator ID3D11ShaderResourceView* () const { return GetTexture(); }
	inline operator bool() { return IsValid(); }
	inline operator bool() const { return IsValid(); }

private:
	bool Commit();

private:
	unsigned char* m_ImageData{ nullptr };

	ID3D11Device* m_pD3DDevice{};
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_SRV{};
	int m_Width{ 0 };
	int m_Height{ 0 };

	std::string m_LastError{};
	bool m_IsLoaded{ false };
};