#pragma once

#include "..//GraphicsPrimitives.h"


namespace jshGraphics_dx11 {

	////////////////////FUNCTIONS///////////////////////////

	bool Initialize();

	jshImGui(bool InitializeImGui());

	bool Close();

	void Begin();
	void End();
	void Present();

	jsh::CommandList BeginCommandList();

#ifdef JSH_IMGUI
	void BeginImGui();
	void EndImGui(const jsh::RenderTargetView& rtv);
#endif

	/////////////////////////TOPOLOGY//////////////////////
	void SetTopology(JSH_TOPOLOGY, jsh::CommandList cmd);

	/////////////////////////BUFFER//////////////////////
	void CreateBuffer(const JSH_BUFFER_DESC* desc, JSH_SUBRESOURCE_DATA* sdata, jsh::Buffer* buffer);

	void BindVertexBuffer(const jsh::Buffer& buffer, uint32 slot, jsh::CommandList cmd);
	void BindIndexBuffer(const jsh::Buffer& buffer, jsh::CommandList cmd);
	void BindConstantBuffer(const jsh::Buffer& buffer, uint32 slot, JSH_SHADER_TYPE constShaderType, jsh::CommandList cmd);

	/////////////////////////INPUTLAYOUT////////////////////////
	void CreateInputLayout(const JSH_INPUT_ELEMENT_DESC* descriptors, uint32 cant, jsh::VertexShader& vs, jsh::InputLayout* il);
	void BindInputLayout(const jsh::InputLayout& inputLayout, jsh::CommandList cmd);

	/////////////////////////SHADER////////////////////////
	void CreateVertexShader(const wchar* path, jsh::VertexShader* vs);
	void CreatePixelShader(const wchar* path, jsh::PixelShader* ps);
	void BindVertexShader(const jsh::VertexShader& vs, jsh::CommandList cmd);
	void BindPixelShader(const jsh::PixelShader& ps, jsh::CommandList cmd);

	/////////////////////////TEXTURE////////////////////////
	void CreateTexture(const JSH_TEXTURE2D_DESC* desc, JSH_SUBRESOURCE_DATA* sdata, jsh::Texture* tex);
	void BindTexture(const jsh::Texture& texture, uint32 slot, JSH_SHADER_TYPE shaderType, jsh::CommandList cmd);

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
	void BindDepthStencilState(const jsh::DepthStencilState& dsState, jsh::CommandList cmd);
	void ClearDepthStencilView(const jsh::Texture& tex, jsh::CommandList cmd);

	/////////////////////////RASTERIZER STATE////////////////////////
	void CreateRasterizerState(const JSH_RASTERIZER_DESC* desc, jsh::RasterizerState* rs);
	void BindRasterizerState(const jsh::RasterizerState& rasterizerState, jsh::CommandList cmd);

	/////////////////////////RENDER TARGET VIEW////////////////////////
	void CreateRenderTargetView(const JSH_RENDER_TARGET_VIEW_DESC* desc, const JSH_TEXTURE2D_DESC* texDesc, jsh::RenderTargetView* rtv);
	void CreateRenderTargetViewFromBackBuffer(jsh::RenderTargetView* rtv);
	void BindRenderTargetView(const jsh::RenderTargetView& rtv, jsh::CommandList cmd);
	void BindRenderTargetView(const jsh::RenderTargetView& rtv, const jsh::Texture& tex, jsh::CommandList cmd);
	void ClearRenderTargetView(const jsh::RenderTargetView& rtv, jsh::CommandList cmd);

	/////////////////////////METHODS////////////////////////////////////////
	void UpdateConstantBuffer(jsh::Buffer& buffer, void* data, jsh::CommandList cmd);

	//////////////////////////DRAW CALLS////////////////////////////////////
	void DrawIndexed(uint32 indicesCount, jsh::CommandList cmd);

}