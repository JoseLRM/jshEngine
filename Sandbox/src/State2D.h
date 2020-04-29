#pragma once

#include "jshEngine.h"

class QuadSystem : public jsh::System {

public:
	float time = 0.f;

	QuadSystem() : System("QuadSystem")
	{
		AddRequestedComponents<jsh::SpriteComponent>();

		//SetExecuteType(JSH_ECS_SYSTEM_MULTITHREADED);
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
			float s = sin(time) * 7.f + 3.5f;
			trans.SetScale({ s, s, 1.f });
			break;
		}
		case 1:
		{
			float s = sin(time) + 0.5f;
			spr->color.x = s;
			spr->color.y = s;
			spr->color.z = 1.f - s;
			break;
		}
		case 2:
		{
			jsh::vec3 pos = trans.GetLocalPosition();
			float s = (sin(time + e) * 5.f) * dt;
			float c = (cos(time + e) * 5.f) * dt;
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

	void CreateEntities(uint32 count) {
		jsh::vector<jsh::Entity> entities;
		jshScene::CreateEntities(count, &entities, jsh::SpriteComponent());

		for (uint32 i = 0; i < entities.size(); ++i) {
			jsh::Transform& transform = jshScene::GetTransform(entities[i]);

			jsh::vec3 pos = transform.GetLocalPosition();
			pos.x = ((float)rand() / RAND_MAX) * 1000.f - 500.f;
			pos.y = ((float)rand() / RAND_MAX) * 700.f - 350.f;
			transform.SetPosition(pos);

			float r = ((float)rand() / RAND_MAX);
			float g = ((float)rand() / RAND_MAX);
			float b = ((float)rand() / RAND_MAX);
			float a = ((float)rand() / RAND_MAX);

			jshScene::GetComponent<jsh::SpriteComponent>(entities[i])->color = { r, g, b, a };

			transform.SetScale({ 20.f, 20.f, 0.f });
			if(i % 2 == 0) jshScene::GetComponent<jsh::SpriteComponent>(entities[i])->sprite.texture = tex;
		}
	}

public:
	void Initialize() override
	{
		jshEngine::SetDefaultRenderer2D();
		jshEngine::GetRenderer()->SetMainCamera(jshScene::CreateEntity(jsh::CameraComponent(), jsh::PostProcessComponent()));
		jsh::CameraComponent* camera = jshScene::GetComponent<jsh::CameraComponent>(jshEngine::GetRenderer()->GetMainCamera());
		camera->SetOrthographicMatrix(1080, 720);
		jshScene::GetComponent<jsh::PostProcessComponent>(jshEngine::GetRenderer()->GetMainCamera())->SetBloomEffect(true);
		
		tex = jshGraphics::CreateTexture("Skybox");
		tex->samplerState = jshGraphics::primitives::GetDefaultSamplerState();
		jshLoader::LoadTexture("res/textures/skybox.jpg", &tex->resource);

		jshEvent::Register<jsh::WindowResizedEvent>(JSH_EVENT_LAYER_DEFAULT, [](jsh::WindowResizedEvent& e) {
		
			jshLogln("Window Resized: %i, %i", e.size.x, e.size.y);

			return true;
		});
	}

	void Update(float dt) override
	{
		if (jshInput::IsKeyPressed('G')) CreateEntities(10000);

		system.time += dt;
		jshScene::UpdateSystem(&system, dt);

		if (jshInput::IsKeyPressed('R')) jshWindow::SetBounds(0, 0, 1080/2, 720/2, false);
	}

	void Render() override
	{

	}

	void Close() override
	{

	}

};