#pragma once

#include "jshEngine.h"
#include "LightGenerator.h"

class SponzaState : public jsh::State {

	jsh::Model model;
	jsh::Entity sponza = jsh::INVALID_ENTITY;
	
	LightGenerator lights;

public:
	SponzaState() {}

	void Load() override
	{
		jshLoader::LoadModel("res/models/Sponza/sponza.obj", "sponza", &model);
		//jshLoader::LoadModel("res/models/jose.fbx", "jose", &model);
	}

	void Initialize() override
	{
		sponza = jshScene::CreateEntity(jsh::NameComponent("Sponza"));
		model.CreateEntity(sponza);

		jsh::LightComponent lightComp;
		lightComp.intensity = 1000.f;
		lightComp.range = 500.f;

		jsh::Entity camera = jshScene::CreateEntity(jsh::CameraComponent(), lightComp);
		jsh::CameraComponent* cameraComp = jshScene::GetComponent<jsh::CameraComponent>(camera);
		cameraComp->GetCamera().SetFieldOfView(90.f);

		jshRenderer::SetCamera(cameraComp);
		jshGraphics::SetResolution(1080);
	}
	void Update(float dt) override
	{
		static bool actived = false;
		if (jshInput::IsKeyPressed('C')) {
			actived = !actived;
			if (actived) jshWindow::HideMouse();
			else jshWindow::ShowMouse();
		}
		if (actived) jshRenderer::GetCamera()->UpdateFirstPerson3D(0.5f, 0.5f, 350.f, 350.f, dt);

		if (jshInput::IsKeyPressed('U')) jshGraphics::SetFullscreen(!jshGraphics::InFullscreen());

		lights.Update(dt);
	}

	void Render() override
	{
		lights.Render();
	}

};