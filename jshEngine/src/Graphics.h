#pragma once

#include "GraphicsPrimitives.h"
#include "GraphicsObjects.h"
#include "GraphicsAdapter.h"

#include "Shaders.h"

namespace jshGraphics {

	namespace _internal {
		bool Initialize();
		bool Close();
		void Present(jsh::RenderTargetView& rtv);

#ifdef JSH_IMGUI
		bool InitializeImGui();
		bool CloseImGui();
		bool ShowImGuiWindow();
#endif
	}

	JSH_GRAPHICS_API GetAPI();

	void SetFullscreen(bool fullscreen);
	bool InFullscreen();

	uint32 GetResolutionWidth();
	uint32 GetResolutionHeight();
	jsh::uvec2 GetResolution();
	float GetResolutionAspect();
	void SetResolution(float width);
	namespace _internal {
		void SetResolutionAspect(float aspect);
	}

	class objects {
		static jsh::SolidShader s_SolidShader;
		static jsh::NormalShader s_NormalShader;

#ifdef JSH_ENGINE
	public:
#endif 
		static void Initialize();

	public:
		inline static jsh::SolidShader* GetSolidShader() { return &s_SolidShader; }
		inline static jsh::NormalShader* GetNormalShader() { return &s_NormalShader; }

	};

}