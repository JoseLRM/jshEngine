#pragma once

#include "jshEngine.h"

class SponzaState : public jsh::State {

	jsh::Entity sponza = jsh::INVALID_ENTITY;

public:
	SponzaState() {}

	void Initialize() override
	{
		sponza = jshScene::CreateEntity(jsh::NameComponent("Sponza"));
		auto model = jshLoader::LoadModel("res/models/Sponza/sponza.obj", "sponza");
		model->CreateEntity(sponza);

		jsh::CameraComponent cameraComp;
		cameraComp.SetPerspectiveMatrix(90.f, 0.2f, 100000.f);
		jsh::LightComponent lightComp;
		lightComp.intensity = 1000.f;
		lightComp.constantAttenuation = 500.f;
		jsh::Entity camera = jshScene::CreateEntity(cameraComp, lightComp);

		jshRenderer::SetCamera(camera);
	}
	void Update(float dt) override
	{
		static bool actived = false;
		if (jshInput::IsKeyPressed('C')) {
			actived = !actived;
			if (actived) jshWindow::HideMouse();
			else jshWindow::ShowMouse();
		}
		if (actived) jshRenderer::GetMainCamera()->UpdateFirstPerson(0.5f, 0.5f, 350.f, 350.f, dt);
	}

};