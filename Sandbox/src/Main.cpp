#include "jshEngine.h"

struct State : public jsh::State {

	void Initialize() override;
	void Update(float dt) override;
	void Render() override;
	void Close() override;

};

int main()
{

	jshEngine::Initialize(new State());
	jshEngine::Run();
	jshEngine::Close();

	return 0;
}

jsh::Mesh* CreateTerrain() {

	jsh::Mesh* mesh = jshGraphics::CreateMesh("Terrain");

	constexpr uint32 resolution = 10;
	constexpr uint32 size = 100;
	constexpr uint32 cantOfVertices = resolution * resolution;
	constexpr uint32 cantOfIndices = (resolution-1) * (resolution-1) * 6;

	float offset = (float)size / (float)resolution;

	struct Color {
		uint8 r, g, b, a = 1.f;
	};

	jsh::vec3 positions[cantOfVertices];
	jsh::vec3 normals[cantOfVertices];
	Color colors[cantOfVertices];
	uint32 indices[cantOfIndices];

	for (uint32 x = 0; x < resolution; ++x) {
		for (uint32 z = 0; z < resolution; ++z) {
			jsh::vec3 position;
			jsh::vec3 normal;
			Color color;
			position.x = (float)x * offset - (float)size / 2.f;
			position.y = sin((float)x/10) + cos((float)z/10);
			position.z = (float)z * offset - (float)size / 2.f;
			normal.x = 0.f;
			normal.y = 1.f;
			normal.z = 0.f;
			color.r = ((float)x / (float)resolution) * 255.f;
			color.g = 50u;
			color.b = ((float)z / (float)resolution) * 255.f;
			positions[x + z * resolution] = position;
			normals[x + z * resolution] = normal;
			colors[x + z * resolution] = color;
		}
	}

	uint32 resolution0 = resolution - 1;;
	for (uint32 x = 0; x < resolution0; ++x) {
		for (uint32 z = 0; z < resolution0; ++z) {
			uint32 i = x + z * resolution0;
			indices[i * 6 + 0] = i;
			indices[i * 6 + 1] = i + 1;
			indices[i * 6 + 2] = i + resolution0;
			indices[i * 6 + 3] = i + resolution0;
			indices[i * 6 + 4] = i + 1;
			indices[i * 6 + 5] = i + resolution0 + 1;
		}
	}

	mesh->SetRawData(jshGraphics::CreateRawData("Terrain"));
	mesh->GetRawData()->SetPositionsAndNormals((float*)positions, (float*)normals, cantOfVertices);
	mesh->GetRawData()->SetColors((uint8*)colors);
	mesh->GetRawData()->SetIndices(indices, cantOfIndices);
	mesh->GetRawData()->Create();

	return mesh;
}

void State::Initialize()
{
	std::shared_ptr<jsh::Model> dragonModel = jshLoader::LoadModel("res/models/nano_textured/nanosuit.obj", "nanosuit");
	//std::shared_ptr<jsh::Model> dragonModel = jshLoader::LoadModel("res/models/Sponza/sponza.obj", "sponza");
	//std::shared_ptr<jsh::Model> dragonModel = jshLoader::LoadModel("res/models/gobber/GoblinX.obj", "Goblin");

	dragonModel->CreateEntity(jshScene::CreateEntity(jsh::NameComponent("Goblin")));
	jsh::Entity cameraEntity = jshScene::CreateEntity(jsh::NameComponent("Camera"), jsh::CameraComponent(), jsh::LightComponent());

	jsh::CameraComponent* camera = jshScene::GetComponent<jsh::CameraComponent>(cameraEntity);
	camera->SetPerspectiveMatrix(70.f, 0.1f, 2000.f);
	jshRenderer::SetCamera(camera);
	jsh::vec3 pos = jshScene::GetTransform(cameraEntity).GetLocalPosition();
	pos.x = -1.f;
	pos.z = -4.f;
	jshScene::GetTransform(cameraEntity).SetPosition(pos);

	jshScene::GetComponent<jsh::LightComponent>(cameraEntity)->intensity = 4.f;

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
		jsh::CameraComponent* camera = jshRenderer::GetMainCamera();
		camera->UpdateFirstPerson(0.5f, 0.5f, 5.f, 5.f, dt);
	}
}

void State::Render()
{
	jsh::MeshComponent* meshComp = jshScene::GetComponent<jsh::MeshComponent>(1);
}

void State::Close()
{

}