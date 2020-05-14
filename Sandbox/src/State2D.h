#pragma once

#include "jshEngine.h"
#include "ParticleGenerator.h"

class State2D : public jsh::State {

	ParticleGenerator particles;
	jsh::Texture* tex;
	jsh::Texture* tex2;
	jsh::Image image;

	bool createParticles = false;

public:
	void Load() override
	{
		tex = jshGraphics::CreateTexture("Skybox");
		tex->samplerState = jshGraphics::primitives::GetDefaultSamplerState();
		jshLoader::LoadTexture("res/textures/skybox.jpg", &tex->resource);

		tex2 = jshGraphics::CreateTexture("Image");
		tex2->samplerState = jshGraphics::primitives::GetDefaultSamplerState();
		// texture
		image = jsh::Image(100, 100, JSH_FORMAT_R8G8B8A8_UNORM);
		jshLoader::CreateTexture(image, &tex2->resource, true);
	}

	void Initialize() override
	{
		jshEngine::GetRenderer()->SetMainCamera(jshScene::CreateEntity(jsh::CameraComponent(), jsh::PostProcessComponent()));
		jsh::CameraComponent* camera = jshScene::GetComponent<jsh::CameraComponent>(jshEngine::GetRenderer()->GetMainCamera());
		camera->SetOrthographicMatrix();
		camera->Set2D();
		camera->SetDimension(10.f);

		particles.Initialize();

		//gui
		jsh::Entity canvas = jshScene::CreateEntity(jsh::GuiCanvasComponent());
		jsh::Entity gui = jshScene::CreateSEntity(canvas, jsh::GuiComponent(), jsh::SpriteComponent(jshColor::RED), jsh::GuiEventComponent());
		jsh::Entity guiChild = jshScene::CreateSEntity(gui, jsh::GuiComponent(), jsh::SpriteComponent(jshColor::BLUE), jsh::GuiEventComponent());
		jsh::GuiComponent* guiComp = jshScene::GetComponent<jsh::GuiComponent>(gui);

		guiComp->SetConstraintX(JSH_GUI_CONSTRAINT_TYPE_PIXEL, 10.f);
		guiComp->SetConstraintY(JSH_GUI_CONSTRAINT_TYPE_PIXEL, 30.f);
		guiComp->ReverseY();
		guiComp->SetConstraintWidth(JSH_GUI_CONSTRAINT_TYPE_RELATIVE, 0.1f);
		guiComp->SetConstraintHeight(JSH_GUI_CONSTRAINT_TYPE_ASPECT, 1.f);
		guiComp->SetLeftCorner();
		guiComp->SetBottomCorner();

		guiComp = jshScene::GetComponent<jsh::GuiComponent>(guiChild);
		guiComp->SetConstraintX(JSH_GUI_CONSTRAINT_TYPE_CENTER);
		guiComp->SetConstraintY(JSH_GUI_CONSTRAINT_TYPE_CENTER);
		guiComp->SetConstraintWidth(JSH_GUI_CONSTRAINT_TYPE_ASPECT, 2.f);
		guiComp->SetConstraintWidth(JSH_GUI_CONSTRAINT_TYPE_RELATIVE, 0.5f);

		jsh::GuiEventComponent* guiClicked = jshScene::GetComponent<jsh::GuiEventComponent>(gui);
		guiClicked->OnClickedFn = [](jsh::Entity entity, jsh::MouseButtonEvent& e) {
			if (e.buttonCode == JSH_MOUSE_LEFT && e.IsPressed()) {
				jsh::Color& color = jshScene::GetComponent<jsh::SpriteComponent>(entity)->color;
				e.Kill();
				color.x+=20;
			}
		};
		guiClicked->OnContactFn = [](jsh::Entity gui, uint8 mode) {
			if (mode == JSH_EVENT_RELEASED) {
				jsh::Color& color = jshScene::GetComponent<jsh::SpriteComponent>(gui)->color;
				color.y += 20;
			}
		};

		guiClicked = jshScene::GetComponent<jsh::GuiEventComponent>(guiChild);
		guiClicked->OnClickedFn = [](jsh::Entity entity, jsh::MouseButtonEvent& e) {
			if (e.buttonCode == JSH_MOUSE_LEFT && e.IsPressed()) {
				jsh::Color& color = jshScene::GetComponent<jsh::SpriteComponent>(entity)->color;
				e.Kill();
				color.x += 20;
			}
		};
		guiClicked->OnContactFn = [](jsh::Entity gui, uint8 mode) {
			if (mode == JSH_EVENT_RELEASED) {
				jsh::Color& color = jshScene::GetComponent<jsh::SpriteComponent>(gui)->color;
				color.y += 20;
			}
		};
		guiClicked->OnFocusFn = [](jsh::Entity gui, uint8 mode, bool& lostFocus) {
		
			jsh::Color& color = jshScene::GetComponent<jsh::SpriteComponent>(gui)->color;
			if (mode == JSH_EVENT_PRESSED) {
				color.z = 255;
			}
			else if (mode == JSH_EVENT_RELEASED) {
				color.z = 0;
			}
			else {
				color.z++;
				if (color.z == 255) lostFocus = true;
			}
			
		};

		jshEvent::Register<jsh::MouseButtonEvent>(JSH_EVENT_LAYER_DEFAULT, [this](jsh::MouseButtonEvent& e) {
			if (e.buttonCode == JSH_MOUSE_CENTER)
			{
				jshFatalError("La cagaste cochatumare %u", 69);
			}

			if (e.buttonCode == JSH_MOUSE_LEFT && e.IsPressed()) createParticles = true;
			else if (e.buttonCode == JSH_MOUSE_LEFT && e.IsReleased()) createParticles = false;
			return true;
		});
	}

	void Update(float dt) override
	{
		jsh::CameraComponent* camera = jshScene::GetComponent<jsh::CameraComponent>(jshEngine::GetRenderer()->GetMainCamera());

		jsh::vec2 mousePos = camera->GetMousePos();
		particles.Update(dt, mousePos.x, mousePos.y, 0.f, createParticles);

		if (jshInput::IsKeyPressed('U')) jshGraphics::SetFullscreen(!jshGraphics::InFullscreen());

		if (jshInput::IsKeyPressed('W')) {
			jshDebug::LogE("Que pasa crack, esto es un error");
		}
	}

	void FixedUpdate() override
	{
		jsh::Color* buffer = reinterpret_cast<jsh::Color*>(image.GetBuffer());
		for (uint32 x = 0; x < image.GetWidth(); ++x) {
			for (uint32 y = 0; y < image.GetHeight(); ++y) {
				buffer[x + (y * image.GetWidth())] = jshColor::GREY(uint8(((float)rand() / RAND_MAX) * 255.f));
			}
		}
		jshGraphics::UpdateTexture(tex2->resource, buffer, 0u, 0u);
	}

	void Render() override
	{
		particles.Render();
	}

	void Close() override
	{

	}

};