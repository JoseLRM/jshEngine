#pragma once

#include "common.h"
#include "State.h"

namespace jsh {

	class DefaultLoadState : public State {
		jsh::Entity entity = INVALID_ENTITY;

	public:
		void Initialize() override
		{
			jsh::Entity camEntity = jshScene::CreateEntity(CameraComponent());
			jsh::CameraComponent* camera = jshScene::GetComponent<CameraComponent>(camEntity);

			Camera& cam = camera->GetCamera();
			cam.Set2D();
			cam.SetOrthographicMatrix();
			cam.SetDimension(10.f);

			jshRenderer::SetCamera(camera);

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