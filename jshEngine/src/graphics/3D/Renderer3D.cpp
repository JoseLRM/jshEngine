#include "Renderer3D.h"

#include "..//..//ecs/Scene.h"
#include "..//..//components/Components.h"
#include "..//Graphics.h"

using namespace jsh;

namespace jshRenderer3D {

	class MeshRendererSystem : public jsh::System {
	public:
		MeshRendererSystem() {
			AddRequestedComponents<Transform3DComponent, MeshComponent>();
		}

		void UpdateEntity(Scene& scene, Entity entity, BaseComponent** components, float dt) override
		{
			Transform3DComponent* transform = (Transform3DComponent*) components[0];
			MeshComponent* mesh = (MeshComponent*) components[1];
			Draw(*mesh->mesh, transform);
		}
	};
	MeshRendererSystem meshRendererSystem;
	class ModelRendererSystem : public jsh::System {
	public:
		ModelRendererSystem() {
			AddRequestedComponents<Transform3DComponent, ModelComponent>();
		}

		void UpdateEntity(Scene& scene, Entity entity, BaseComponent** components, float dt) override
		{
			Transform3DComponent* transform = (Transform3DComponent*)components[0];
			ModelComponent* modelComp = (ModelComponent*)components[1];
			Model& model = *modelComp->model;
			for (uint32 i = 0; i < model.meshes.size(); ++i) {
				Draw(*model.meshes[i], transform);
			}
		}
	};
	ModelRendererSystem modelRendererSystem;

	Buffer cvBuffer;
	alignas(16) struct {
		XMMATRIX tm;
		XMMATRIX pm;
	} cvData;

	Buffer cpBuffer;
	alignas(16) struct {
		vec4 lightPos;
		struct {
			float quadratic;
			float linear;
			float constant;
		}att;
		float intensity;
		vec4 color;
	} cpData;

	bool Initialize()
	{
		cvData.pm = XMMatrixTranspose(XMMatrixPerspectiveFovLH(70, 1080.f / 720.f, 5.f, 2000.f));
		cvBuffer = jshGraphics::CreateBuffer(&cvData, sizeof(cvData), 0, JSH_USAGE_DEFAULT, 0u, JSH_BUFFER_TYPE_CONSTANT, JSH_SHADER_TYPE_VERTEX);

		cpData.color.x = 1.f;
		cpData.color.y = 1.f;
		cpData.color.z = 1.f;
		cpData.intensity = 10.f;
		cpData.lightPos.x = 0.f;
		cpData.lightPos.y = 0.f;
		cpData.lightPos.z = 0.f;
		cpData.att.constant = 1.f;
		cpData.att.linear = 0.3f;
		cpData.att.quadratic = 0.016f;
		cpBuffer = jshGraphics::CreateBuffer(&cpData, sizeof(cpData), 0, JSH_USAGE_DEFAULT, 0u, JSH_BUFFER_TYPE_CONSTANT, JSH_SHADER_TYPE_PIXEL);
		return true;
	}

	bool Close()
	{
		return true;
	}

	void DrawScene(Scene* pScene)
	{
		Scene& scene = *pScene;

		System* systems[] = {
			&meshRendererSystem,
			&modelRendererSystem,
		};
		pScene->UpdateSystems(systems, 2, 0.f);

	}

	void Draw(jsh::Mesh& mesh, jsh::Transform3DComponent* transform)
	{
		cvData.tm = XMMatrixTranspose(XMMatrixRotationX(transform->rotation.x * 0.0174533f) * XMMatrixRotationY(transform->rotation.y * 0.0174533f) * XMMatrixRotationZ(transform->rotation.z * 0.0174533f)
			* XMMatrixTranslation(transform->position.x, transform->position.y, transform->position.z));
		jshGraphics::UpdateConstantBuffer(cvBuffer, &cvData);

		jshGraphics::BindConstantBuffer(cvBuffer);
		jshGraphics::BindConstantBuffer(cpBuffer);

		mesh.Bind();
		jshGraphics::DrawIndexed(mesh.GetIndexCount());
	}

}