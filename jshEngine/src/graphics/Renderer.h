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
#endif

	void SetCamera(jsh::CameraComponent* camera);
	jsh::CameraComponent* GetMainCamera();

}