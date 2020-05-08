#pragma once

#include "State.h"
#include "Engine.h"
#include "Renderer3D.h"

namespace jsh {

	class DefaultLoadState : public State {
		jsh::Entity entity = INVALID_ENTITY;

	public:
		void Initialize() override
		{
			jsh::Renderer* renderer = jshEngine::GetRenderer();

			jsh::CameraComponent camera;
			camera.Set2D();
			camera.SetOrthographicMatrix();
			camera.SetDimension(10.f);

			renderer->SetMainCamera(jshScene::CreateEntity(camera));

			entity = jshScene::CreateEntity(jsh::SpriteComponent());
			Transform& trans = jshScene::GetTransform(entity);
			trans.SetScale({ 4.f, 4.f, 1.f });
		}

		void Update(float dt) override
		{
			Transform& trans = jshScene::GetTransform(entity);
			float rot = ToDegrees(trans.GetLocalRotation().z);
			rot += 50.f * dt;
			trans.SetRotation({ 0.f, 0.f, ToRadians(rot) });
		}

		void Close() override
		{
			jshScene::ClearScene();
		}

	};

}