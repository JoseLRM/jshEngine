#pragma once

#include "common.h"
#include "Technique.h"

namespace jsh {
	class RenderGraph;
	class Camera;
	class MeshRenderer;
	class SpriteRenderer;

	class Renderer : public Technique {};

}

namespace jshRenderer {

	namespace _internal {
		bool Initialize();
		bool Close();
		void Begin();
		void Render();
		void End();
#ifdef JSH_IMGUI
		bool ShowImGuiWindow();
#endif
	}

	//////////////GETTERS AND SETTERS//////////////////////
	void SetRenderGraph(jsh::RenderGraph* rg);
	jsh::RenderGraph* GetRenderGraph();

	void SetCamera(jsh::CameraComponent* camera);
	jsh::CameraComponent* GetCamera();

	namespace environment {
		void SetAmbientLight(float r, float g, float b);
		jsh::vec3 GetAmbientLight();
	}

	jsh::SpriteRenderer& GetSpriteRenderer();
	jsh::MeshRenderer& GetMeshRenderer();

}