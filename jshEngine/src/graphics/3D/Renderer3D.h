#pragma once

#include "..//..//common.h"
#include "..//GraphicsObjects.h"

namespace jsh {
	class Scene;
	class Transform3DComponent;
}

namespace jshRenderer3D {

#ifdef JSH_ENGINE
	bool Initialize();
	bool Close();
#endif

	void DrawScene(jsh::Scene* pScene);

	void Draw(jsh::Mesh& mesh, jsh::Transform3DComponent* transform);

}