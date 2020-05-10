#pragma once

#include "jshEngine.h"
#include "ParticleGenerator.h"

class State2D : public jsh::State {

	ParticleGenerator particles;
	jsh::Texture* tex;
	jsh::Texture* tex2;

public:
	void Load() override
	{
		tex = jshGraphics::CreateTexture("Skybox");
		tex->samplerState = jshGraphics::primitives::GetDefaultSamplerState();
		jshLoader::LoadTexture("res/textures/skybox.jpg", &tex->resource);

		tex2 = jshGraphics::CreateTexture("Skybox2");
		tex2->samplerState = jshGraphics::primitives::GetDefaultSamplerState();
		jshLoader::LoadTexture("res/textures/skybox2.png", &tex2->resource);
	}

	void Initialize() override
	{
		jshEngine::GetRenderer()->SetMainCamera(jshScene::CreateEntity(jsh::CameraComponent(), jsh::PostProcessComponent()));
		jsh::CameraComponent* camera = jshScene::GetComponent<jsh::CameraComponent>(jshEngine::GetRenderer()->GetMainCamera());
		camera->SetOrthographicMatrix();
		camera->Set2D();
		camera->SetDimension(10.f);
		jshScene::GetComponent<jsh::PostProcessComponent>(jshEngine::GetRenderer()->GetMainCamera())->SetBloomEffect(true);

		particles.Initialize();
	}

	void Update(float dt) override
	{
		jsh::CameraComponent* camera = jshScene::GetComponent<jsh::CameraComponent>(jshEngine::GetRenderer()->GetMainCamera());

		bool create = jshInput::IsMouse(JSH_MOUSE_LEFT);

		jsh::vec2 mousePos = camera->GetMousePos();
		particles.Update(dt, mousePos.x, mousePos.y, 0.f, create);

		if (jshInput::IsKeyPressed('U')) jshGraphics::SetFullscreen(!jshGraphics::InFullscreen());
	}

	void Render() override
	{
		jshImGui(particles.ShowImGuiWindow());
	}

	void Close() override
	{

	}

};