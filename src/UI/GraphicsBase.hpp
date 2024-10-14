#pragma once

#include <d3d11_4.h>
#include <wrl/client.h>
#include <vector>
#include <optional>
#include <memory>
#include <string>

#define GLM_FORCE_ROW_MAJOR
#include "glm/glm.hpp"

namespace Graphics 
{
	using Microsoft::WRL::ComPtr;

#pragma pack(push, 1)

	struct Vertex_t {
		glm::vec3	Pos;
		glm::vec2	UV;
		glm::vec4	Col;
	};

#pragma pack(pop)

	struct BACKUP_DX11_STATE
    {
        UINT                        ScissorRectsCount, ViewportsCount;
        D3D11_RECT                  ScissorRects[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
        D3D11_VIEWPORT              Viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
        ID3D11RasterizerState*      RS;
        ID3D11BlendState*           BlendState;
        FLOAT                       BlendFactor[4];
        UINT                        SampleMask;
        UINT                        StencilRef;
        ID3D11DepthStencilState*    DepthStencilState;
        ID3D11ShaderResourceView*   PSShaderResource;
        ID3D11SamplerState*         PSSampler;
        ID3D11PixelShader*          PS;
        ID3D11VertexShader*         VS;
        ID3D11GeometryShader*       GS;
        UINT                        PSInstancesCount, VSInstancesCount, GSInstancesCount;
        ID3D11ClassInstance         *PSInstances[256], *VSInstances[256], *GSInstances[256];   // 256 is max according to PSSetShader documentation
        D3D11_PRIMITIVE_TOPOLOGY    PrimitiveTopology;
        ID3D11Buffer*               IndexBuffer, *VertexBuffer, *VSConstantBuffer;
        UINT                        IndexBufferOffset, VertexBufferStride, VertexBufferOffset;
        DXGI_FORMAT                 IndexBufferFormat;
        ID3D11InputLayout*          InputLayout;
    };

	struct Rect {
		float L, T, R, B;
	};

	class ErrorHandled
	{
	public:
		virtual ~ErrorHandled() = default;

		bool HasFailed() { return FAILED(m_ErrorCode); }
		bool HasSucceeded() { return SUCCEEDED(m_ErrorCode); }
		const auto& GetLastErrorMsg() const { return m_ErrorStr; }
		const auto& GetLastErrorCode() const { return m_ErrorCode; }

	protected:
		bool ErrorCheck(HRESULT hr, std::string file, std::string function, size_t line);

	private:
		void SetError(HRESULT hr, std::string position);

		HRESULT m_ErrorCode{ S_OK };
		std::string m_ErrorStr{};
	};

	class Drawable : public ErrorHandled
	{
	public:
		virtual ~Drawable();

		const auto& GetVertices() const { return m_Vertices; }
		const auto& GetIndices() const { return m_Indices; }
		const auto& GetVertexLayout() const { return s_VertexInputLayout; }

		bool AllocateGPUVertexBuffer(ID3D11Device* pD3D11Device, _Out_ ID3D11Buffer** ppVertexBuffer,
			const void* pShaderBytecodeWithInputSignature, size_t bytecodeLength,
			D3D11_USAGE usage = D3D11_USAGE_DEFAULT, UINT cpuAccessFlags = 0);
		bool AllocateGPUIndexBuffer(ID3D11Device* pD3D11Device, _Out_ ID3D11Buffer** ppIndexBuffer,
			const void* pShaderBytecodeWithInputSignature, size_t bytecodeLength,
			D3D11_USAGE usage = D3D11_USAGE_DEFAULT, UINT cpuAccessFlags = 0);
		bool CreateGPUInputLayout(ID3D11Device* pD3D11Device, void* pShaderBytecodeWithInputSignature,
			size_t bytecodeLength, ID3D11InputLayout** ppInputLayout);

		// Transformations
		
		void Move(glm::vec3 vector); // Normalized
		void Scale(glm::vec3 vector); // Normalized
		void Scale(float factor); // Normalized
		void Rotate(float angle, glm::vec3 vector); // Normalized

		const auto& GetTransformationmatrix() const { return m_TransformationMatrix; }

	protected:
		Drawable(const std::vector<Vertex_t>& vertices, const std::vector<uint32_t>& indices);

	private:
		void UpdateTransformationMatrix();

	private:
		std::vector<Vertex_t> m_Vertices{};
		std::vector<uint32_t> m_Indices{};

		glm::f32vec3 m_TranslationVector{ 0.0f, 0.0f, 0.0f };
		float		 m_RotationAngle{ 0.0f }; // Radians
		glm::f32vec3 m_RotationAxisVector{ 0.0f, 1.0f, 0.0f };
		glm::f32vec3 m_ScaleVector{ 1.0f, 1.0f, 1.0f };

		glm::mat4 m_TransformationMatrix{ glm::mat4(1.0f) };

		static constexpr D3D11_INPUT_ELEMENT_DESC s_VertexInputLayout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, (UINT)offsetof(Vertex_t, Pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, (UINT)offsetof(Vertex_t, UV),  D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, (UINT)offsetof(Vertex_t, Col), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
	};
}