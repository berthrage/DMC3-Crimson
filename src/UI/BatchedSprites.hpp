#pragma once

#include <array>

#include "GraphicsBase.hpp"
#include "Sprite.hpp"
#include "Texture2DArrayD3D11.hpp"

#include <span>

namespace Graphics {
    class BatchedSprites : public ErrorHandled
    {
    public:
        enum class RatioFlags : int 
        {
            StretchToVP = 0,
            KeepOriginalSize = 1 << 0,
            FitToVPHeight = 1 << 1,
            FitToVPWidth = 1 << 2,
            FitToVP = 1 << 3,
        };

        enum class MaskType_t
        {
            None = 0,
            SlantedMask = 1,
            RadialMask = 2,

            SIZE = 3
        };

        struct MaskInfo
        {
            MaskType_t Type;

            union
            {
                struct // Slanted mask
                {
                    glm::f32vec2 Origin;
                    float        LineNormalDeg;
                    glm::f32vec2 SoftnessRange;
                } Slanted;

                struct // Radial mask
                {
                    glm::f32vec2 Origin;
                    float        Radius;
                    float        StartDeg;
                    float        EndDeg;
                    bool         KeepPositive;
                    bool         FlipMask;
                } Radial;
            };
        };

    private:
        class IndexUpdateQueue
        {
        public:
            IndexUpdateQueue() = default;
            ~IndexUpdateQueue() = default;
            
        public:
            inline void ResizeList(size_t newSize) { m_DirtyFlags.resize(newSize); };
            inline void QueueAllForUpdate() { m_UpdateAllQueued = true; }
            inline void QueueUpdate(size_t idx) { m_UpdateQueued = true; m_DirtyFlags[idx] = true; m_DirtyIdxs.push_back(idx); }
            inline bool IsUpdateQueued() const { return m_UpdateQueued || m_UpdateAllQueued; }
            inline bool ConsumeUpdate(size_t idx) { bool res = m_DirtyFlags[idx]; m_DirtyFlags[idx] = false; return res; }
            inline bool IsDirty(size_t idx) { return m_DirtyFlags[idx]; }
            inline const auto& GetDirtyIndices() const { return m_DirtyIdxs; }
            inline void DequeueUpdate() { m_UpdateAllQueued = m_UpdateQueued = false; m_DirtyIdxs.clear(); };
            inline bool IsUpdateAllQueued() const { return m_UpdateAllQueued; }

        private:
            bool                m_UpdateAllQueued{ false };
            bool                m_UpdateQueued{ false };
            std::vector<size_t> m_DirtyIdxs{};
            std::vector<bool>   m_DirtyFlags{};
        };

    	struct SpriteInfo
        {
    		size_t		 TextureIdx;
    		float		 Opacity;
    		float		 Brightness;
            glm::f32vec4 Tint; // RGBA
            glm::f32vec4 RectRegion; // LTRB
            uint32_t     MaskStartIdx;
            uint32_t     MaskCount;
    		glm::f32vec3 Translation;
            glm::f32vec3 Rotation;
            glm::f32vec3 Scale;
    	};
    
    #pragma pack(push, 1)
    	struct alignas(16) VERTEX_SHADER_SPRITEINSTANCE_INFO
        {
    		uint32_t	 TextureIdx;
    		float		 Opacity;
    		float		 Brightness;
            glm::f32vec2 TextureDims;
            glm::f32vec4 Tint; // RGBA
            glm::f32vec4 RectRegion; // LTRB
            uint32_t     MaskStartIdx;
            uint32_t     MaskCount;
            glm::mat4x4	 Transform;
    	};
    
    	struct alignas(16) VERTEX_CONSTANT_BUFFER
        {
    		alignas(16) glm::mat4 MVP;
            float                 Opacity;
            float                 Brightness;
            glm::f32vec2          VPDims;
            int                   RatioFlags;
    	};
    
    	struct alignas(16) PIXEL_CONSTANT_BUFFER
        {
    		uint32_t TextureCount;
    		uint32_t SamplerCount;
            uint32_t MaskCount;
    	};

        struct alignas(16) PIXEL_MASK_INFO
        {
            uint32_t Type;
            uint32_t _pad0;
            uint32_t _pad1;
            uint32_t _pad2;

            union
            {
                struct // Slanted mask
                {
                    glm::f32vec2 Origin;
                    glm::f32vec2 DirNormal;
                    glm::f32vec2 SoftnessRange;
                } Slanted; // 24 bytes

                struct // Radial mask
                {
                    glm::f32vec2 Origin;
                    float        RadiusSquared;
                    glm::f32vec2 StartDir;
                    glm::f32vec2 EndDir;
                    float        KeepPositive; // 1.0f or 0.0f for shader math speed
                    float        FlipMask; // 1.0f or 0.0f for shader math speed
                } Radial; // 36 bytes

                float Params[12]; // 48 byte size ensurance
            };
        };

        static_assert(sizeof(PIXEL_MASK_INFO) == 64, "PIXEL_MASK_INFO struct size misaligned! (16 byte alignment required)");
    #pragma pack(pop)
    
    public:
        BatchedSprites(ID3D11Device* pD3D11Device, UINT width, UINT height, const std::vector<SpriteDesc>& spriteDescs,
            std::shared_ptr<Texture2DArrayD3D11> pSpriteTextures, const std::vector<size_t> spriteIndices);
    
    	virtual ~BatchedSprites();
    
    	/// <summary>
    	///     Reads the sprite info list provided and prepares the necessary data for rendering,
        ///     if the last argument (spriteTextures) is NOT provided it will load the the textures
        ///     into video memory from disk, but if provided it will not try to reload the textures from the disk which is slow
    	/// </summary>
    	/// <param name="pD3D11Device">Pointer to the ID3D11Device to use for rendering</param>
    	/// <param name="spriteDescs">A vector tha contains the SpriteDesc objects that contain
        /// the information about the image of each sprite </param>
    	/// <param name="spriteIndices">(Optional) A list of indices of the sprites to be batched and rendered,
        /// if not provided, it will default to using all of the sprites</param>
    	/// <param name="pSpriteTextureArray">(Optional) A pointer to the already loaded into vram sprite texture array,
        /// if not provided it will load the sprites into vram by reading them from the disk which is slow</param>
    	void UpdateSprites(ID3D11Device* pD3D11Device, const std::vector<SpriteDesc>& spritesInfo,
            const std::vector<size_t> spriteIndices, std::shared_ptr<Texture2DArrayD3D11> pSpriteTextureArray);
    
    	void ChangeSprites(ID3D11Device* pD3D11Device, const std::vector<SpriteDesc>& spriteDescs,
            const std::vector<size_t> spriteIndices, std::shared_ptr<Texture2DArrayD3D11> pSpriteTextureArray);
    
    	bool Initialize(ID3D11Device* pD3D11Device, UINT width, UINT height);
    	bool Draw(ID3D11DeviceContext* pD3D11DeviceContext);
    
        void AddMask(size_t spriteIdx, const MaskInfo& maskInfo);
        const MaskInfo& GetSpriteMaskInfo(size_t spriteIdx, size_t maskIdx);
        std::span<const MaskInfo> GetSpriteMasksInfo(size_t spriteIdx);
        void SetSpriteMasks(size_t spriteIdx, const std::vector<MaskInfo>& maskInfos);
        void UpdateSpriteMask(size_t spriteIdx, size_t maskIdx, const MaskInfo& maskInfo);
        void RemoveSpriteMask(size_t spriteIdx, size_t maskIdx);

    	void Translate(size_t idx, glm::vec3 vector);
    	void TranslateTo(size_t idx, glm::vec3 vector);
    	void Rotate(size_t idx, glm::vec3 rotation);
    	void RotateTo(size_t idx, glm::vec3 rotation);
    	void Scale(size_t idx, glm::vec3 vector);
    	void ScaleTo(size_t idx, glm::vec3 vector);
        void Tint(size_t idx, glm::vec4 tintCol);
        void TintTo(size_t idx, glm::vec4 tintCol);
        void SetRatioFlags(int flags) { m_RatioFlags = flags; }
    
    	void SetTransform(size_t idx, glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale);
    	void SetOpacity(size_t idx, float opacity);
    	void SetBrightness(size_t idx, float brightness);
    
        void SetVPOpacity(float opacity) { m_Opacity = opacity; } // Opacity of the whole batch
        void SetVPBrightness(float brightness) { m_Brightness = brightness; } // Brightness of the whole batch

        const auto& GetVPOpacity() const { return m_Opacity; };
        const auto& GetVPBrightness() const { return m_Brightness; };

        const auto& GetTranslation(size_t idx) const { return m_SpriteInstanceData[idx].Translation; }
        const auto& GetRotation(size_t idx) const { return m_SpriteInstanceData[idx].Rotation; }
        const auto& GetScale(size_t idx) const { return m_SpriteInstanceData[idx].Scale; }
        const auto& GetOpacity(size_t idx) const { return m_SpriteInstanceData[idx].Opacity; }
        const auto& GetBrightness(size_t idx) const { return m_SpriteInstanceData[idx].Brightness; }
        const auto& GetTint(size_t idx) const { return m_SpriteInstanceData[idx].Tint; }
        const auto& GetRatioFlags() const { return m_RatioFlags; }

        const auto& GetDims() const { return m_RTDims; }
    
    	//void Reorder(const std::vector<size_t>& spriteIndices);
    
    	const auto& GetCustomIndices() { return m_SpriteIndices; }
    
    	const auto GetRTSRV() const { return m_pRenderTargetSRV.Get(); };
    
    private:
    	//void SortByDepth();
    
    	bool CreateRenderTargetView(ID3D11Device* pD3D11Device, UINT width, UINT height);
    
    	bool CreateVertexBuffer(ID3D11Device* pD3D11Device);
    	bool CreateIndexBuffer(ID3D11Device* pD3D11Device);
    	bool CreateAndUpdateSpriteInfoBuffer(ID3D11Device* pD3D11Device);
        bool CreateAndUpdateMaskInfoBuffer(ID3D11Device* pD3D11Device);
    
        bool UpdateSpriteInstanceInfoBuffer(ID3D11DeviceContext* pD3D11DeviceContext);
    	bool UpdateVertexConstantBuffer(ID3D11DeviceContext* pD3D11DeviceContext, const VERTEX_CONSTANT_BUFFER& cBuffer);
    	bool UpdatePixelConstantBuffer(ID3D11DeviceContext* pD3D11DeviceContext, const PIXEL_CONSTANT_BUFFER& cBuffer);
        bool UpdatePixelMaskBuffer(ID3D11DeviceContext* pD3D11DeviceContext);
    
    	//bool CreateSpriteInfoBufferSrv(ID3D11Device* pD3D11Device);
        bool CreateMaskInfoBufferSrv(ID3D11Device* pD3D11Device);
    
    	void BackupD3D11State(ID3D11DeviceContext* ctx);
    	void RestoreD3D11State(ID3D11DeviceContext* ctx);
    
    private:
    	glm::u32vec2 m_RTDims{ 0L };
        float        m_Opacity{ 1.0f }, m_Brightness{ 1.0f };
        int          m_RatioFlags{ 0 };

        IndexUpdateQueue m_SpriteUpdateInfo{}, m_MaskUpdateInfo{};

    	std::vector<Vertex_t> m_Vertices{};
    
        // Indices given by the user, to know which sprites inside the texture array to render
    	std::vector<size_t> m_SpriteIndices{};
    
    	D3D11_VIEWPORT m_ViewPort{};
    
    	BACKUP_DX11_STATE m_D3D11StateBackup;
    
    	ComPtr<ID3D11RenderTargetView>	 m_pRenderTargetView{};
    	ComPtr<ID3D11ShaderResourceView> m_pRenderTargetSRV{};
    	ComPtr<ID3D11VertexShader>		 m_pVertexShader{};
    	ComPtr<ID3D11InputLayout>		 m_pInputLayout{};
    	ComPtr<ID3D11PixelShader>		 m_pPixelShader{};
    	ComPtr<ID3D11Buffer>			 m_pVertexBuffer{};
    	ComPtr<ID3D11Buffer>			 m_pIndexBuffer{};
    	ComPtr<ID3D11Buffer>			 m_pVertexConstantBuffer{};
    	ComPtr<ID3D11Buffer>			 m_pPixelConstantBuffer{};
    	ComPtr<ID3D11Buffer>			 m_pSpriteInstanceInfoBuffer{};
        ComPtr<ID3D11Buffer>			 m_pPixelMaskInfoStructuredBuffer{};
    	ComPtr<ID3D11ShaderResourceView> m_pInstanceInfoBufferSRV{};
        ComPtr<ID3D11ShaderResourceView> m_pMaskInfoBufferSRV{};
    	ComPtr<ID3D11BlendState>		 m_pBlendState{};
    	ComPtr<ID3D11RasterizerState>	 m_pRasterizerState{};
    	ComPtr<ID3D11DepthStencilState>	 m_pDepthStencilStateOpaque{};
    	ComPtr<ID3D11DepthStencilState>	 m_pDepthStencilStateTransparent{};
    	ComPtr<ID3D11Texture2D>			 m_pDepthStincilBuffer{};
    	ComPtr<ID3D11DepthStencilView>	 m_pDepthStincilView{};
    	ComPtr<ID3D11SamplerState>		 m_pTextureSampler{};
    
    	std::shared_ptr<Texture2DArrayD3D11> m_pTextureArray{};
    	std::vector<SpriteInfo>				 m_SpriteInstanceData{};
        std::vector<MaskInfo>				 m_MaskInfoData{};
    
    	size_t m_CurrentVBSize{ 0 };
    	size_t m_CurrentIBSize{ 0 };
    	size_t m_CurrentSpriteInstanceInfoBufferSize{ 0 };
        size_t m_CurrentMaskInfoBufferSize{ 0 };

		std::vector<VERTEX_SHADER_SPRITEINSTANCE_INFO> m_InstanceInfoBufferArray;
        std::vector<PIXEL_MASK_INFO>                   m_MaskInfoBufferArray;
		//std::vector<VERTEX_SHADER_SPRITEINSTANCE_INFO> m_InfoArray;
    
    private: // Statics
        static constexpr D3D11_INPUT_ELEMENT_DESC s_VertexInputLayout[] =
        {
        // Slot 0 - per-vertex data
            { "POSITION",       0, DXGI_FORMAT_R32G32B32_FLOAT,     0, (UINT)offsetof(Vertex_t, Pos),                                   D3D11_INPUT_PER_VERTEX_DATA,   0 }, // 0 for advance per vertex
            { "TEXCOORD",       0, DXGI_FORMAT_R32G32_FLOAT,        0, (UINT)offsetof(Vertex_t, UV),                                    D3D11_INPUT_PER_VERTEX_DATA,   0 }, // 0 for advance per vertex
            { "COLOR",          0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0, (UINT)offsetof(Vertex_t, Col),                                   D3D11_INPUT_PER_VERTEX_DATA,   0 }, // 0 for advance per vertex

        // Slot 1 - per-instance data
            { "TEXTURE_IDX",    0, DXGI_FORMAT_R32_UINT,            1, (UINT)offsetof(VERTEX_SHADER_SPRITEINSTANCE_INFO, TextureIdx),   D3D11_INPUT_PER_INSTANCE_DATA, 1 }, // 1 for advance per instance
            { "OPACITY",        0, DXGI_FORMAT_R32_FLOAT,           1, (UINT)offsetof(VERTEX_SHADER_SPRITEINSTANCE_INFO, Opacity),      D3D11_INPUT_PER_INSTANCE_DATA, 1 }, // 1 for advance per instance
            { "BRIGHTNESS",     0, DXGI_FORMAT_R32_FLOAT,           1, (UINT)offsetof(VERTEX_SHADER_SPRITEINSTANCE_INFO, Brightness),   D3D11_INPUT_PER_INSTANCE_DATA, 1 }, // 1 for advance per instance
            { "TEXDIMS",        0, DXGI_FORMAT_R32G32_FLOAT,        1, (UINT)offsetof(VERTEX_SHADER_SPRITEINSTANCE_INFO, TextureDims),  D3D11_INPUT_PER_INSTANCE_DATA, 1 }, // 1 for advance per instance
            { "TINT",           0, DXGI_FORMAT_R32G32B32A32_FLOAT,  1, (UINT)offsetof(VERTEX_SHADER_SPRITEINSTANCE_INFO, Tint),         D3D11_INPUT_PER_INSTANCE_DATA, 1 }, // 1 for advance per instance
            { "RECT",           0, DXGI_FORMAT_R32G32B32A32_FLOAT,  1, (UINT)offsetof(VERTEX_SHADER_SPRITEINSTANCE_INFO, RectRegion),   D3D11_INPUT_PER_INSTANCE_DATA, 1 }, // 1 for advance per instance
            { "MASK_START_IDX", 0, DXGI_FORMAT_R32_UINT,            1, (UINT)offsetof(VERTEX_SHADER_SPRITEINSTANCE_INFO, MaskStartIdx), D3D11_INPUT_PER_INSTANCE_DATA, 1 }, // 1 for advance per instance
            { "MASK_COUNT",     0, DXGI_FORMAT_R32_UINT,            1, (UINT)offsetof(VERTEX_SHADER_SPRITEINSTANCE_INFO, MaskCount),    D3D11_INPUT_PER_INSTANCE_DATA, 1 }, // 1 for advance per instance

            // Transformation matrix (split into 4 float4s)
            { "TRANSFORM",      0, DXGI_FORMAT_R32G32B32A32_FLOAT,  1, (UINT)(offsetof(VERTEX_SHADER_SPRITEINSTANCE_INFO, Transform) + sizeof(float) * 0),  D3D11_INPUT_PER_INSTANCE_DATA, 1 }, // 1 for advance per instance
            { "TRANSFORM",      1, DXGI_FORMAT_R32G32B32A32_FLOAT,  1, (UINT)(offsetof(VERTEX_SHADER_SPRITEINSTANCE_INFO, Transform) + sizeof(float) * 4),  D3D11_INPUT_PER_INSTANCE_DATA, 1 }, // 1 for advance per instance
            { "TRANSFORM",      2, DXGI_FORMAT_R32G32B32A32_FLOAT,  1, (UINT)(offsetof(VERTEX_SHADER_SPRITEINSTANCE_INFO, Transform) + sizeof(float) * 8),  D3D11_INPUT_PER_INSTANCE_DATA, 1 }, // 1 for advance per instance
            { "TRANSFORM",      3, DXGI_FORMAT_R32G32B32A32_FLOAT,  1, (UINT)(offsetof(VERTEX_SHADER_SPRITEINSTANCE_INFO, Transform) + sizeof(float) * 12), D3D11_INPUT_PER_INSTANCE_DATA, 1 }, // 1 for advance per instance
        };

        static constexpr std::array<uint32_t, 6> s_SpriteIndices{ 0, 1, 2, 0, 2, 3 };
    };
}
