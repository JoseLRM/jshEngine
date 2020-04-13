#pragma once

#include "..//ecs/Scene.h"
#include "..//graphics/GraphicsObjects.h"

namespace jsh {

	struct MeshComponent : public Component<MeshComponent> {

		Mesh* mesh;

		MeshComponent() : mesh(nullptr) {}
		MeshComponent(Mesh* mesh) : mesh(mesh) {}
		MeshComponent(const char* meshName) : mesh(jshGraphics::GetMesh(meshName)) {}

#ifdef JSH_IMGUI
	public:
		void ShowInfo() override
		{
			if (mesh) {
				if(ImGui::Button("Mesh")) jshDebug::ShowMeshImGuiWindow(mesh);
			}
		}
#endif
	
	};
	jshDefineComponent(MeshComponent);

}