#pragma once

#include "GraphicsPrimitives.h"
#include "GraphicsObjects.h"

namespace jshGraphics {

#ifdef JSH_ENGINE
	bool Initialize();
	bool Close();

#ifdef JSH_IMGUI
	bool InitializeImGui();
#endif
#endif

	JSH_GRAPHICS_API GetAPI();

	void Begin();
	void End();
	void Present(uint32 interval);

#ifdef JSH_IMGUI
	void BeginImGui();
	void EndImGui(const jsh::RenderTargetView& rtv);
#endif

	void SetResolution(uint32 width, uint32 height);
	jsh::uvec2 GetResolution();
	void SetFullscreen(bool fullscreen);
	bool InFullscreen();

	bool GenerateInputLayout(jsh::Shader* shader, jsh::RawData* rawData, jsh::InputLayout* il);

	class objects {
		static jsh::SolidShader s_SolidShader;
		static jsh::NormalShader s_NormalShader;
		static jsh::Shader s_SpriteShader;

		static jsh::RawData s_SpriteRawData;

#ifdef JSH_ENGINE
	public:
#endif 
		static void Initialize();

	public:
		inline static jsh::Shader* GetSpriteShader() { return &s_SpriteShader; }
		inline static jsh::SolidShader* GetSolidShader() { return &s_SolidShader; }
		inline static jsh::NormalShader* GetNormalShader() { return &s_NormalShader; }


		inline static jsh::RawData* GetSpriteRawData() { return &s_SpriteRawData; }
	};

}