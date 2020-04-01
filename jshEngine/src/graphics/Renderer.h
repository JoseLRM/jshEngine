#pragma once

#include "..//common.h"
#include "GraphicsObjects.h"
#include "Skybox.h"

namespace jsh {
	class Scene;
	class CameraComponent;
}

namespace jshRenderer {

#ifdef JSH_ENGINE
	bool Initialize();
	bool Close();
#endif

	void DrawScene(jsh::Scene* pScene);
	void SetCamera(jsh::CameraComponent* camera);
	void SetSkybox(const jsh::Skybox& skybox);

}