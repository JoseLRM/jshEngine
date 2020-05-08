#include "jshEngine.h"

#include "SponzaState.h"
#include "State2D.h"

struct State : public jsh::State {

	jsh::Model model;

	void Load() override;
	void Initialize() override;
	void Update(float dt) override;
	void Render() override;
	void Close() override;

};

int main()
{
	jshEngine::Initialize(new State2D());
	jshEngine::Run();
	jshEngine::Close();

	return 0;
}

void State::Load()
{
	//jshLoader::LoadModel("res/models/nano_textured/nanosuit.obj", "nanosuit", &model);
	jshLoader::LoadModel("res/models/gobber/GoblinX.obj", "Goblin", &model);
}

void State::Initialize()
{
	jsh::Entity goblin = jshScene::CreateEntity(jsh::NameComponent("Goblin"));
	model.CreateEntity(goblin);
	jsh::Entity cameraEntity = jshScene::CreateEntity(jsh::NameComponent("Camera"), jsh::CameraComponent(), jsh::LightComponent());

	jshScene::GetComponent<jsh::LightComponent>(cameraEntity)->intensity = 4.f;

	jsh::CameraComponent* camera = jshScene::GetComponent<jsh::CameraComponent>(cameraEntity);
	jshEngine::GetRenderer()->SetMainCamera(cameraEntity);
	jsh::vec3 pos = jshScene::GetTransform(cameraEntity).GetLocalPosition();
	pos.x = -1.f;
	pos.z = -4.f;
	jshScene::GetTransform(cameraEntity).SetPosition(pos);

	jshScene::GetComponent<jsh::LightComponent>(cameraEntity)->intensity = 4.f;

	for (float x = 0.f; x < 10.f; x++)
	{
		for (float y = 0.f; y < 10.f; y++)
		{
			for (float z = 0.f; z < 10.f; z++)
			{
				if (x == 0.f && y == 0.f && z == 0.f) continue;

				jsh::Entity entity = jshScene::DuplicateEntity(goblin);
				jsh::Transform& trans = jshScene::GetTransform(entity);
				trans.SetPosition({ x * 5.f, y * 5.f, z * 5.f });
			}
		}
	}
}


void State::Update(float dt)
{
	static bool actived = false;
	if (jshInput::IsKeyPressed('C')) {
		actived = !actived;
		if (actived) jshWindow::HideMouse();
		else jshWindow::ShowMouse();
	}
	if (actived) {
		jsh::CameraComponent* camera = jshScene::GetComponent<jsh::CameraComponent>(jshEngine::GetRenderer()->GetMainCamera());
		camera->UpdateFirstPerson3D(0.5f, 0.5f, 5.f, 5.f, dt);
	}

	if (jshInput::IsKeyPressed(JSH_KEY_F5)) 
		jshEngine::LoadState(new State2D());

	if (jshInput::IsKeyPressed('R')) jshGraphics::SetResolution(1080 / 2, 720 / 2);
	if (jshInput::IsKeyPressed('T')) jshGraphics::SetResolution(1080, 720);
	if (jshInput::IsKeyPressed('Y')) jshGraphics::SetResolution(1080 * 2, 720 * 2);
	if (jshInput::IsKeyPressed('U')) jshGraphics::SetFullscreen(!jshGraphics::InFullscreen());
}

void State::Render()
{
	jsh::MeshComponent* meshComp = jshScene::GetComponent<jsh::MeshComponent>(1);
}

void State::Close()
{
	jshScene::ClearScene();
}