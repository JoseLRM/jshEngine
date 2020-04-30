#pragma once

#include "jshEngine.h"

class SponzaState : public jsh::State {

	jsh::Entity sponza = jsh::INVALID_ENTITY;

public:
	SponzaState() {}

	void Initialize() override
	{
		sponza = jshScene::CreateEntity(jsh::NameComponent("Sponza"));
		jsh::Model model;
		jshLoader::LoadModel("res/models/Sponza/sponza.obj", "sponza", &model);
		model.CreateEntity(sponza);

		jsh::CameraComponent cameraComp;
		cameraComp.SetFieldOfView(90.f);
		jsh::LightComponent lightComp;
		lightComp.intensity = 1000.f;
		lightComp.constantAttenuation = 500.f;
		jsh::Entity camera = jshScene::CreateEntity(cameraComp, lightComp);

		jshEngine::GetRenderer()->SetMainCamera(camera);
	}
	void Update(float dt) override
	{
		static bool actived = false;
		if (jshInput::IsKeyPressed('C')) {
			actived = !actived;
			if (actived) jshWindow::HideMouse();
			else jshWindow::ShowMouse();
		}
		if (actived) jshScene::GetComponent<jsh::CameraComponent>(jshEngine::GetRenderer()->GetMainCamera())->UpdateFirstPerson3D(0.5f, 0.5f, 350.f, 350.f, dt);

		if (jshInput::IsKeyPressed('R')) jshGraphics::SetResolution(1080 / 2, 720 / 2);
		if (jshInput::IsKeyPressed('T')) jshGraphics::SetResolution(1080, 720);
		if (jshInput::IsKeyPressed('Y')) jshGraphics::SetResolution(1080 * 2, 720 * 2);
		if (jshInput::IsKeyPressed('U')) jshGraphics::SetFullscreen(!jshGraphics::InFullscreen());
	}

};