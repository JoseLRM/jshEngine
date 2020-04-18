#pragma once

#include "..//common.h"
#include "GraphicsObjects.h"

namespace jsh {
	class CameraComponent;
}

namespace jshRenderer {

#ifdef JSH_ENGINE
	bool Initialize();
	bool Close();

	void BeginFrame();
	void EndFrame();

#ifdef JSH_IMGUI
	bool ShowImGuiWindow();
#endif

#endif

	void SetCamera(jsh::CameraComponent* camera);
	jsh::CameraComponent* GetMainCamera();

	namespace primitives {
		jsh::RenderTargetView& GetMainRenderTargetView();
		jsh::DepthStencilState& GetDefaultDepthStencilState();
		jsh::Resource& GetDefaultDepthStencilView();

		jsh::RenderTargetView& GetOffscreenRenderTargetView();

		jsh::SamplerState& GetDefaultSamplerState();
		jsh::Viewport& GetDefaultViewport();

		jsh::Resource& GetCameraBuffer();
		jsh::Resource& GetLightBuffer();
	}

}