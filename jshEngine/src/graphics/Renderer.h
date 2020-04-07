#pragma once

#include "..//common.h"
#include "GraphicsObjects.h"

namespace jsh {
	class Scene;
	class CameraComponent;
}

namespace jshRenderer {

#ifdef JSH_ENGINE
	bool Initialize();
	bool Close();

	void BeginFrame();
	void EndFrame();
#endif

	void DrawScene(jsh::Scene* pScene);
	void SetCamera(jsh::CameraComponent* camera);

}