#pragma once

#include "GraphicsDesc.h"

namespace jsh {

	struct RenderState;

	typedef uint32 CommandList;

	struct GraphicsPrimitive {
		std::shared_ptr<void> internalAllocation;
		inline bool IsValid() const noexcept { return internalAllocation.get() != nullptr; }
	};

	struct Resource : public GraphicsPrimitive {};

	struct InputLayout : public GraphicsPrimitive {};

	struct VertexShader : public GraphicsPrimitive {};
	struct PixelShader : public GraphicsPrimitive {};
	struct GeometryShader : public GraphicsPrimitive {};
	struct HullShader : public GraphicsPrimitive {};
	struct DomainShader : public GraphicsPrimitive {};
	struct ConstantShader : public GraphicsPrimitive {};

	struct Viewport : public GraphicsPrimitive {};

	struct RenderTargetView : public GraphicsPrimitive {};
	struct DepthStencilState : public GraphicsPrimitive {};
	struct SamplerState : public GraphicsPrimitive {};
	struct BlendState : public GraphicsPrimitive {};
	struct RasterizerState : public GraphicsPrimitive {};
}

namespace jshGraphics {

	jsh::CommandList BeginCommandList();

	/////////////////////////BUFFER//////////////////////
	void CreateResource(const JSH_BUFFER_DESC* desc, JSH_SUBRESOURCE_DATA* sdata, jsh::Resource* buffer);

	void BindVertexBuffer(const jsh::Resource& buffer, uint32 slot, jsh::CommandList cmd);
	void BindIndexBuffer(const jsh::Resource& buffer, jsh::CommandList cmd);
	void BindConstantBuffer(const jsh::Resource& buffer, uint32 slot, JSH_SHADER_TYPE constShaderType, jsh::CommandList cmd);

	/////////////////////////INPUTLAYOUT////////////////////////
	void CreateInputLayout(const JSH_INPUT_ELEMENT_DESC* descriptors, uint32 cant, jsh::VertexShader& vs, jsh::InputLayout* il);
	void BindInputLayout(const jsh::InputLayout& inputLayout, jsh::CommandList cmd);

	/////////////////////////SHADER////////////////////////
	void CreateVertexShader(const wchar* path, jsh::VertexShader* vs);
	void CreatePixelShader(const wchar* path, jsh::PixelShader* ps);
	void BindVertexShader(const jsh::VertexShader& vs, jsh::CommandList cmd);
	void BindPixelShader(const jsh::PixelShader& ps, jsh::CommandList cmd);

	/////////////////////////TEXTURE////////////////////////
	void CreateResource(const JSH_TEXTURE2D_DESC* desc, JSH_SUBRESOURCE_DATA* sdata, jsh::Resource* tex);
	void BindTexture(const jsh::Resource& texture, uint32 slot, JSH_SHADER_TYPE shaderType, jsh::CommandList cmd);
	void BindTexture(const jsh::RenderTargetView& rtv, uint32 slot, JSH_SHADER_TYPE shaderType, jsh::CommandList cmd);
	void UnbindTexture(uint32 slot, JSH_SHADER_TYPE shaderType, jsh::CommandList cmd);

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
	void ClearDepthStencilView(const jsh::Resource& tex, jsh::CommandList cmd);

	/////////////////////////RASTERIZER STATE////////////////////////
	void CreateRasterizerState(const JSH_RASTERIZER_DESC* desc, jsh::RasterizerState* rs);
	void BindRasterizerState(const jsh::RasterizerState& rasterizerState, jsh::CommandList cmd);

	/////////////////////////RENDER TARGET VIEW////////////////////////
	void CreateRenderTargetView(const JSH_RENDER_TARGET_VIEW_DESC* desc, const JSH_TEXTURE2D_DESC* texDesc, jsh::RenderTargetView* rtv);
	void CreateRenderTargetViewFromBackBuffer(JSH_RENDER_TARGET_VIEW_DESC* desc, jsh::RenderTargetView* rtv);
	void BindRenderTargetView(const jsh::RenderTargetView& rtv, jsh::CommandList cmd);
	void BindRenderTargetView(const jsh::RenderTargetView& rtv, const jsh::Resource& tex, jsh::CommandList cmd);
	void ClearRenderTargetView(const jsh::RenderTargetView& rtv, float r, float g, float b, float a, jsh::CommandList cmd);

	const jsh::RenderState& GetRenderState();

}