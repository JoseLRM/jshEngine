#pragma once

#include "GraphicsDesc.h"

namespace jshGraphics {
	namespace _internal {
		template<typename T>
		struct GraphicsPrimitive {
			T* internalAllocation = nullptr;
			inline bool IsValid() const noexcept { return internalAllocation; }
			inline void Release() noexcept
			{
				internalAllocation->Release();
				delete internalAllocation;
				internalAllocation = nullptr;
			}
		};

		struct GraphicsPrimitive_internal {
			virtual void Release() = 0;
		};

		struct Resource_Internal : public GraphicsPrimitive_internal {
			JSH_RESOURCE_TYPE type = JSH_RESOURCE_TYPE_INVALID;
		};
		struct Buffer_Internal : public Resource_Internal {
			JSH_BUFFER_DESC desc;
		};
		struct TextureRes_Internal : public Resource_Internal {
			JSH_TEXTURE2D_DESC desc;
		};

		struct InputLayout_Internal : public GraphicsPrimitive_internal {};

		struct VertexShader_Internal : public GraphicsPrimitive_internal {};
		struct PixelShader_Internal : public GraphicsPrimitive_internal {};
		struct GeometryShader_Internal : public GraphicsPrimitive_internal {};
		struct HullShader_Internal : public GraphicsPrimitive_internal {};
		struct DomainShader_Internal : public GraphicsPrimitive_internal {};
		struct ComputeShader_Internal : public GraphicsPrimitive_internal {};

		struct Viewport_Internal : public GraphicsPrimitive_internal {};

		struct RenderTargetView_Internal : public GraphicsPrimitive_internal {
			JSH_RENDER_TARGET_VIEW_DESC desc;
			JSH_TEXTURE2D_DESC resDesc;
		};
		struct DepthStencilState_Internal : public GraphicsPrimitive_internal {};
		struct SamplerState_Internal : public GraphicsPrimitive_internal {};
		struct BlendState_Internal : public GraphicsPrimitive_internal {};
		struct RasterizerState_Internal : public GraphicsPrimitive_internal {};
	}
}

namespace jsh {

	typedef uint32 CommandList;

	typedef jshGraphics::_internal::GraphicsPrimitive<jshGraphics::_internal::Resource_Internal>			Resource;
	typedef jshGraphics::_internal::GraphicsPrimitive<jshGraphics::_internal::Buffer_Internal>				Buffer;
	typedef jshGraphics::_internal::GraphicsPrimitive<jshGraphics::_internal::TextureRes_Internal>			TextureRes;
			
	typedef jshGraphics::_internal::GraphicsPrimitive<jshGraphics::_internal::InputLayout_Internal>			InputLayout;
			
	typedef jshGraphics::_internal::GraphicsPrimitive<jshGraphics::_internal::VertexShader_Internal>		VertexShader;
	typedef jshGraphics::_internal::GraphicsPrimitive<jshGraphics::_internal::PixelShader_Internal>			PixelShader;
	typedef jshGraphics::_internal::GraphicsPrimitive<jshGraphics::_internal::GeometryShader_Internal>		GeometryShader;
			
	typedef jshGraphics::_internal::GraphicsPrimitive<jshGraphics::_internal::Viewport_Internal>			Viewport;

	typedef jshGraphics::_internal::GraphicsPrimitive<jshGraphics::_internal::RenderTargetView_Internal>	RenderTargetView;
	typedef jshGraphics::_internal::GraphicsPrimitive<jshGraphics::_internal::DepthStencilState_Internal>	DepthStencilState;
	typedef jshGraphics::_internal::GraphicsPrimitive<jshGraphics::_internal::SamplerState_Internal>		SamplerState;
	typedef jshGraphics::_internal::GraphicsPrimitive<jshGraphics::_internal::BlendState_Internal>			BlendState;
	typedef jshGraphics::_internal::GraphicsPrimitive<jshGraphics::_internal::RasterizerState_Internal>		RasterizerState;

	struct VertexProperty {
		const char* name;
		JSH_FORMAT format;
		uint32 index;
		inline bool operator==(const VertexProperty& other)
		{
			return name == other.name && format == other.format && index == other.index;
		}
		VertexProperty(const char* name, JSH_FORMAT format, uint32 index) : name(name), format(format), index(index) {}
	};

}

namespace jshGraphics {

	namespace _internal {
		void ReleasePrimitives();
	}

	jsh::CommandList BeginCommandList();

	/////////////////////////DRAW CALLS//////////////////////
	void Draw(uint32 vertexCount, jsh::CommandList cmd);
	void DrawIndexed(uint32 indicesCount, jsh::CommandList cmd);
	void DrawInstanced(uint32 vertexPerInstance, uint32 instances, uint32 startVertex, uint32 startInstance, jsh::CommandList cmd);
	void DrawIndexedInstanced(uint32 indexPerInstance, uint32 instances, uint32 startIndex, uint32 startVertex, uint32 startInstance, jsh::CommandList cmd);

	/////////////////////////TOPOLOGY//////////////////////
	void SetTopology(JSH_TOPOLOGY topology, jsh::CommandList cmd);

	/////////////////////////BUFFER//////////////////////
	void CreateBuffer(const JSH_BUFFER_DESC* desc, JSH_SUBRESOURCE_DATA* sdata, jsh::Buffer* buffer);

	void BindVertexBuffers(const jsh::Buffer* buffers, uint32 slot, uint32 count, const uint32* strides, const uint32* offsets, jsh::CommandList cmd);
	void BindIndexBuffer(const jsh::Buffer& buffer, JSH_FORMAT format, uint32 offset, jsh::CommandList cmd);
	void BindConstantBuffers(const jsh::Buffer* buffers, uint32 slot, uint32 count, JSH_SHADER_TYPE shaderType, jsh::CommandList cmd);

	const JSH_BUFFER_DESC& GetBufferDesc(const jsh::Buffer& buffer);

	void UpdateBuffer(jsh::Buffer& res, void* data, uint32 size, jsh::CommandList cmd);

	/////////////////////////INPUTLAYOUT////////////////////////
	void CreateInputLayout(const JSH_INPUT_ELEMENT_DESC* descriptors, uint32 cant, jsh::VertexShader& vs, jsh::InputLayout* il);
	void BindInputLayout(const jsh::InputLayout& inputLayout, jsh::CommandList cmd);

	/////////////////////////SHADER////////////////////////
	void CreateVertexShader(const wchar* path, jsh::VertexShader* vs);
	void CreatePixelShader(const wchar* path, jsh::PixelShader* ps);
	void BindVertexShader(const jsh::VertexShader& vs, jsh::CommandList cmd);
	void BindPixelShader(const jsh::PixelShader& ps, jsh::CommandList cmd);

	/////////////////////////TEXTURE////////////////////////
	void CreateTextureRes(const JSH_TEXTURE2D_DESC* desc, JSH_SUBRESOURCE_DATA* sdata, jsh::TextureRes* tex);
	void BindTexture(const jsh::TextureRes& texture, uint32 slot, JSH_SHADER_TYPE shaderType, jsh::CommandList cmd);
	void BindTexture(const jsh::RenderTargetView& rtv, uint32 slot, JSH_SHADER_TYPE shaderType, jsh::CommandList cmd);
	void UnbindTexture(uint32 slot, JSH_SHADER_TYPE shaderType, jsh::CommandList cmd);

	void UpdateTexture(jsh::TextureRes& res, void* data, uint32 size, jsh::CommandList cmd);

	const JSH_TEXTURE2D_DESC& GetTextureDesc(const jsh::TextureRes& res);

	/////////////////////////VIEWPORT////////////////////////
	void CreateViewport(float x, float y, float width, float height, jsh::Viewport* vp);
	void BindViewport(const jsh::Viewport& viewport, uint32 slot, jsh::CommandList cmd);

	/////////////////////////SAMPLER STATE////////////////////////
	void CreateSamplerState(const JSH_SAMPLER_DESC* desc, jsh::SamplerState* ss);
	void BindSamplerState(const jsh::SamplerState& ss, uint32 slot, JSH_SHADER_TYPE shaderType, jsh::CommandList cmd);

	/////////////////////////BLEND STATE////////////////////////
	void CreateBlendState(const JSH_BLEND_DESC* desc, jsh::BlendState* bs);
	void BindBlendState(const jsh::BlendState& blendState, jsh::CommandList cmd);

	/////////////////////////DEPTHSTENCIL STATE////////////////////////
	void CreateDepthStencilState(const JSH_DEPTH_STENCIL_DESC* desc, jsh::DepthStencilState* dss);
	void BindDepthStencilState(const jsh::DepthStencilState& dsState, uint32 stencilRef, jsh::CommandList cmd);
	void ClearDepthStencilView(const jsh::TextureRes& tex, jsh::CommandList cmd);

	/////////////////////////RASTERIZER STATE////////////////////////
	void CreateRasterizerState(const JSH_RASTERIZER_DESC* desc, jsh::RasterizerState* rs);
	void BindRasterizerState(const jsh::RasterizerState& rasterizerState, jsh::CommandList cmd);

	/////////////////////////RENDER TARGET VIEW////////////////////////
	void CreateRenderTargetView(const JSH_RENDER_TARGET_VIEW_DESC* desc, const JSH_TEXTURE2D_DESC* texDesc, jsh::RenderTargetView* rtv);
	void BindRenderTargetView(const jsh::RenderTargetView& rtv, jsh::CommandList cmd);
	void BindRenderTargetView(const jsh::RenderTargetView& rtv, const jsh::TextureRes& tex, jsh::CommandList cmd);
	void ClearRenderTargetView(const jsh::RenderTargetView& rtv, float r, float g, float b, float a, jsh::CommandList cmd);

	void ResizeRenderTargetView(jsh::RenderTargetView& rtv, uint32 width, uint32 height);

	const JSH_RENDER_TARGET_VIEW_DESC& GetRenderTargetViewDesc(const jsh::RenderTargetView& rtv);
	const JSH_TEXTURE2D_DESC& GetRenderTargetTextureDesc(const jsh::RenderTargetView& rtv);

	jsh::RenderTargetView& GetRenderTargetViewFromBackBuffer();

	//////////////////////////DEFAULT PRIMITIVES///////////////////////
	class primitives {
		static jsh::DepthStencilState s_DefaultDepthStencilState;
		static jsh::DepthStencilState s_DisabledDepthStencilState;

		static jsh::TextureRes s_DefaultDepthStencilView;
		static jsh::RenderTargetView s_OffscreenRenderTargetView;

		static jsh::SamplerState s_DefaultSamplerState;
		static jsh::Viewport s_DefaultViewport;

		static jsh::BlendState s_TransparentBlendState;
		static jsh::BlendState s_DefaultBlendState;

#ifdef JSH_ENGINE
	public:
#endif
		static void Initialize();

	public:		
		inline static jsh::DepthStencilState& GetDefaultDepthStencilState() { return s_DefaultDepthStencilState; }
		inline static jsh::DepthStencilState& GetDisabledDepthStencilState() { return s_DisabledDepthStencilState; }
		inline static jsh::TextureRes& GetDefaultDepthStencilView() { return s_DefaultDepthStencilView; }

		inline static jsh::RenderTargetView& GetOffscreenRenderTargetView() { return s_OffscreenRenderTargetView; }

		inline static jsh::SamplerState& GetDefaultSamplerState() { return s_DefaultSamplerState; }
		inline static jsh::Viewport& GetDefaultViewport() { return s_DefaultViewport; }

		inline static jsh::BlendState& GetTransparentBlendState() { return s_TransparentBlendState; }
		inline static jsh::BlendState& GetDefaultBlendState() { return s_TransparentBlendState; }
	};
}