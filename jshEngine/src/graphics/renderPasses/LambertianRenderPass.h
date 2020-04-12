#pragma once

#include "RenderPass.h"
#include "..//..//ecs/System.h"

namespace jsh {

	class Scene;
	class CameraComponent;

#define JSH_GFX_MAX_LIGHTS 120
	struct Light {
		vec4 lightPos;
		vec4 data;
		vec4 color;
	};

	struct LightData {
		Light lights[JSH_GFX_MAX_LIGHTS];
	};

	struct MeshInstance {
		Mesh* mesh;
		Transform* pTransform;
	};

	class MeshSystem : public System {
	public:

		jsh::vector<MeshInstance>* pList;

		void UpdateEntity(Entity e, BaseComponent** comp, float dt) override;
	};
	class LightSystem : public System {
	public:

		uint32 count = 0u;
		LightData* pLightData;

		void UpdateEntity(Entity e, BaseComponent** comp, float dt) override;
		void Clear();
	};

	struct MatrixData {
		XMMATRIX tm;
		XMMATRIX vm;
		XMMATRIX pm;
	};

	class LambertianRenderPass : public RenderPass {

		MeshSystem m_MeshSystem;
		LightSystem m_LightSystem;
		jsh::vector<MeshInstance> m_Instances;

		Buffer m_MatrixBuffer;
		MatrixData m_MatrixData;
		Buffer m_LightBuffer;
		LightData m_LightData;

	public:
		LambertianRenderPass();

		void Create() override;

	protected:
		void Render() override;

	};

}