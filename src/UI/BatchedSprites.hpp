#pragma once

#include "GraphicsBase.hpp"
#include "Sprite.hpp"
#include "Texture2DArrayD3D11.hpp"

namespace Graphics 
{
	class BatchedSprites : public ErrorHandled
	{
	private:
		struct SpriteInfo {
			size_t		TexturePathIdx;
			size_t		VerticesOffset;
			size_t		IndicesOffset;
			float		Opacity;
			float		Brightness;
			glm::mat4	TransformationMatrix;
		};

#pragma pack(push, 1)
		struct VERTEX_SHADER_SPRITE_INFO {
			uint32_t	TextureIdx;
			float		Opacity;
			float		Brightness;
			glm::mat4	TransformationMatrix;
		};

		__declspec(align(16)) struct VERTEX_CONSTANT_BUFFER {
			glm::mat4 MVP;
			uint32_t VertexPerSprite;
			uint32_t IndexPerSprite;
		};

		__declspec(align(16)) struct PIXEL_CONSTANT_BUFFER {
			uint32_t TextureCount;
			uint32_t SamplerCount;
		};
#pragma pack(pop)

	public:
		BatchedSprites(ID3D11Device* pD3D11Device, UINT width, UINT height, const std::vector<Sprite>& sprites,
			const std::vector<size_t> spriteIndices = {});
		virtual ~BatchedSprites();

		void UpdateSprites(ID3D11Device* pD3D11Device, const std::vector<Sprite>& sprites,
			const std::vector<size_t> spriteIndices = {});
		void SetActiveSprites(const std::vector<size_t> spriteIndices = {});

		bool Initialize(ID3D11Device* pD3D11Device, UINT width, UINT height);
		bool Draw(ID3D11DeviceContext* pD3D11DeviceContext);

		void Translate(size_t idx, glm::vec3 vector);
		void TranslateTo(size_t idx, glm::vec3 vector);
		void Rotate(size_t idx, float rotation, glm::vec3 axis);
		void RotateTo(size_t idx, float rotation, glm::vec3 axis);
		void Scale(size_t idx, glm::vec3 vector);
		void ScaleTo(size_t idx, glm::vec3 vector);

		void SetTransform(size_t idx, glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale);
		void SetOpacity(size_t idx, float opacity);
		void SetBrightness(size_t idx, float brightness);

		void Reorder(const std::vector<size_t>& spriteIndices);

		const auto GetRTSRV() const { return m_pRTSRV.Get(); };

	private:
		std::optional<size_t> FindTexturePathIndex(const std::string& path);
		void PopulateActiveIndices();
		//void SortByDepth();

		bool CreateRenderTargetView(ID3D11Device* pD3D11Device, UINT width, UINT height);

		bool CreateAndUpdateVertexBuffer(ID3D11Device* pD3D11Device);
		bool CreateAndUpdateIndexBuffer(ID3D11Device* pD3D11Device);
		bool CreateAndUpdateSpriteInfoBuffer(ID3D11Device* pD3D11Device);

		bool UpdateVertexBuffer(ID3D11DeviceContext* pD3D11DeviceContext);
		bool UpdateIndexBuffer(ID3D11DeviceContext* pD3D11DeviceContext);
		bool UpdateSpriteInfoBuffer(ID3D11DeviceContext* pD3D11DeviceContext);
		bool UpdateVertexConstantBuffer(ID3D11DeviceContext* pD3D11DeviceContext, const VERTEX_CONSTANT_BUFFER& cBuffer);
		bool UpdatePixelConstantBuffer(ID3D11DeviceContext* pD3D11DeviceContext, const PIXEL_CONSTANT_BUFFER& cBuffer);

		bool CreateSpriteInfoBufferSrv(ID3D11Device* pD3D11Device);

		void BackupD3D11State(ID3D11DeviceContext* ctx);
		void RestoreD3D11State(ID3D11DeviceContext* ctx);

	private:
		static constexpr D3D11_INPUT_ELEMENT_DESC s_VertexInputLayout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, (UINT)offsetof(Vertex_t, Pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, (UINT)offsetof(Vertex_t, UV),  D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, (UINT)offsetof(Vertex_t, Col), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		static constexpr size_t s_VertexPerSprite{ 4 }, s_IndexPerSprite{ 6 };

		UINT m_RTWidth{ 0 }, m_RTHeight{ 0 };

		bool m_UpdateVertexBuffer{ false };
		bool m_UpdateIndexBuffer{ false };
		bool m_SpriteInfoUpdateQueued{ false };

		std::vector<Vertex_t> m_Vertices{};
		std::vector<uint32_t> m_Indices{};
		std::vector<uint32_t> m_ActiveIndices{};

		std::vector<size_t> m_CustomSpriteIndices{};

		D3D11_VIEWPORT m_VP{};

		BACKUP_DX11_STATE m_D3D11StateBackup;

		ComPtr<ID3D11RenderTargetView>		m_pRTV{};
		ComPtr<ID3D11ShaderResourceView>	m_pRTSRV{};
		ComPtr<ID3D11VertexShader>			m_pVertexShader{};
		ComPtr<ID3D11InputLayout>			m_pInputLayout{};
		ComPtr<ID3D11PixelShader>			m_pPixelShader{};
		ComPtr<ID3D11Buffer>				m_pVertexBuffer{};
		ComPtr<ID3D11Buffer>				m_pIndexBuffer{};
		ComPtr<ID3D11Buffer>				m_pVertexConstantBuffer{};
		ComPtr<ID3D11Buffer>				m_pPixelConstantBuffer{};
		ComPtr<ID3D11Buffer>				m_pSpriteInfoBuffer{};
		ComPtr<ID3D11ShaderResourceView>	m_pTIBSRV{};
		ComPtr<ID3D11BlendState>			m_pBlendState{};
		ComPtr<ID3D11RasterizerState>		m_pRasterizerState{};
		ComPtr<ID3D11DepthStencilState>		m_pDepthStencilStateOpaque{};
		ComPtr<ID3D11DepthStencilState>		m_pDepthStencilStateTransparent{};
		ComPtr<ID3D11Texture2D>				m_pDepthStincilBuffer{};
		ComPtr<ID3D11DepthStencilView>		m_pDepthStincilView{};
		ComPtr<ID3D11SamplerState>			m_pTextureSampler{};

		std::vector<std::string>				m_TexturePaths{};
		std::unique_ptr<Texture2DArrayD3D11>	m_pTextureArray{};
		std::vector<SpriteInfo>					m_SpritesData{};

		size_t m_CurrentVBSize{ 0 };
		size_t m_CurrentIBSize{ 0 };
		size_t m_CurrentSIBSize{ 0 };
	};
}