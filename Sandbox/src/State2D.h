#pragma once

#include "jshEngine.h"

class QuadSystem : public jsh::System {

public:
	float time = 0.f;

	QuadSystem() : System("QuadSystem")
	{
		AddRequestedComponent(jsh::SpriteComponent::ID);
	}

	void UpdateEntity(jsh::Entity e, jsh::BaseComponent** comp, float dt) override
	{
		jsh::SpriteComponent* spr = reinterpret_cast<jsh::SpriteComponent*>(comp[0]);
		jsh::Transform& trans = jshScene::GetTransform(e);

		uint32 mode = e % 4;
		switch (mode)
		{
		case 0:
		{
			float s = sin(time) + .5f;
			trans.SetScale({ s, s, 1.f });
			break;
		}
		case 1:
		{
			byte s = sin(time) * 255 + 255 / 2;
			spr->color.x = s;
			spr->color.y = s;
			spr->color.z = 255 - s;
			break;
		}
		case 2:
		{
			jsh::vec3 pos = trans.GetLocalPosition();
			float s = (sin(time + e) * 1.f) * dt;
			float c = (cos(time + e) * 1.f) * dt;
			pos.x += s;
			pos.y += c;
			trans.SetPosition({ pos.x, pos.y, 0.f });
			break;
		}
		case 3:
		{
			float s = (sin(time) * 10.f + 5.f) * dt;
			float r = trans.GetLocalRotation().z;
			r += s;
			trans.SetRotation({ 0.f, 0.f, r });
			break;
		}
		}

	}

};

class State2D : public jsh::State {

	QuadSystem system;
	jsh::Texture* tex;
	jsh::Texture* tex2;

	jsh::Renderer2D renderer;

	void CreateEntities(uint32 count) {
		jsh::vector<jsh::Entity> entities;
		jshScene::CreateEntities(count, &entities, jsh::SpriteComponent());

		for (uint32 i = 0; i < entities.size(); ++i) {
			jsh::Transform& transform = jshScene::GetTransform(entities[i]);

			jsh::vec3 pos = transform.GetLocalPosition();
			pos.x = ((float)rand() / RAND_MAX) * 2 - 1.f;
			pos.y = ((float)rand() / RAND_MAX) * 2 - 1.f;
			transform.SetPosition(pos);

			byte r = ((float)rand() / RAND_MAX) * 255;
			byte g = ((float)rand() / RAND_MAX) * 255;
			byte b = ((float)rand() / RAND_MAX) * 255;
			byte a = ((float)rand() / RAND_MAX) * 100 + 155;

			jshScene::GetComponent<jsh::SpriteComponent>(entities[i])->color = { r, g, b, a };

			if (i % 3 == 0) jshScene::GetComponent<jsh::SpriteComponent>(entities[i])->sprite.texture = tex;
			else if (i % 3 == 1) jshScene::GetComponent<jsh::SpriteComponent>(entities[i])->sprite.texture = tex2;
		}
	}

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
		jshEngine::SetRenderer(&renderer);
		jshEngine::GetRenderer()->SetMainCamera(jshScene::CreateEntity(jsh::CameraComponent(), jsh::PostProcessComponent()));
		jsh::CameraComponent* camera = jshScene::GetComponent<jsh::CameraComponent>(jshEngine::GetRenderer()->GetMainCamera());
		camera->SetOrthographicMatrix();
		camera->Set2D();
		camera->SetDimension(10.f);
		jshScene::GetComponent<jsh::PostProcessComponent>(jshEngine::GetRenderer()->GetMainCamera())->SetBloomEffect(true);

		jshEvent::Register<jsh::ResolutionEvent>(JSH_EVENT_LAYER_DEFAULT, [](jsh::ResolutionEvent& e) {

			jshLogln("Resolution: %i, %i", e.resolution.x, e.resolution.y);

			return true;
		});
	}

	void Update(float dt) override
	{
		if (jshInput::IsKeyPressed('G')) CreateEntities(1000);

		if (jshInput::IsKey('E')) system.SetExecuteType(JSH_ECS_SYSTEM_MULTITHREADED);
		else system.SetExecuteType(JSH_ECS_SYSTEM_SAFE);
		system.time += dt;
		jshScene::UpdateSystem(&system, dt);

		if (jshInput::IsKeyPressed('R')) jshGraphics::SetResolution(1080 / 2, 720 / 2);
		if (jshInput::IsKeyPressed('T')) jshGraphics::SetResolution(1080, 720);
		if (jshInput::IsKeyPressed('Y')) jshGraphics::SetResolution(1080 * 2, 720 * 2);
		if (jshInput::IsKeyPressed('U')) jshGraphics::SetFullscreen(!jshGraphics::InFullscreen());
	}

	void Render() override
	{

	}

	void Close() override
	{

	}

};