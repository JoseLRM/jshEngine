#pragma once

#include "..//ecs/Scene.h"
#include "..//graphics/GraphicsObjects.h"

namespace jsh {

	struct MeshComponent : public Component<MeshComponent> {

		Mesh* mesh;

		MeshComponent() {}
		MeshComponent(Mesh* mesh) : mesh(mesh) {}
		MeshComponent(const char* meshName) : mesh(jshGraphics::GetMesh(meshName)) {}

#ifdef JSH_IMGUI
	private:
		bool diffuse = false;
		bool normal = false;
	public:
		void ShowInfo() override
		{
			if (!mesh) return;
			if (mesh->HasDiffuseMap()) {
				ImGui::Checkbox("DiffuseMapping", &diffuse);
				mesh->EnableDiffuseMap(diffuse);
			}
			if (mesh->HasNormalMap()) {
				ImGui::Checkbox("NormalMapping", &normal);
				mesh->EnableNormalMap(normal);
			}
		}
#endif
	
	};
	jshDefineComponent(MeshComponent);

}