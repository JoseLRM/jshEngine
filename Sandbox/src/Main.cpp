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

	mesh->rawData = jshGraphics::CreateRawData("Terrain");
	mesh->rawData->SetPositionsAndNormals((float*)positions, (float*)normals, cantOfVertices);
	mesh->rawData->SetColors((uint8*)colors);
	mesh->rawData->SetIndices(indices, cantOfIndices);
	mesh->rawData->Create();

	return mesh;
}

void State::Initialize()
{
	CreateTerrain();
	std::shared_ptr<jsh::Model> dragonModel = jshLoader::LoadModel("res/models/", "jose.obj");
	jshGraphics::Save("DragonModel", dragonModel);

	jsh::Texture texture;
	jshLoader::LoadTexture("res/models/pene.jpg", &texture);

	//dragonModel.mesh->SetDiffuseMap(texture);

	dragonModel->CreateEntity(jshScene::CreateEntity(jsh::NameComponent("Dragon"), jsh::TransformComponent(-5.f, -5.f, 20.f)));
	jshScene::CreateEntity(jsh::NameComponent("Pene"), jsh::TransformComponent(5.f, -5.f, 20.f), jsh::MeshComponent("Terrain"));
	jsh::Entity cameraEntity = jshScene::CreateEntity(jsh::NameComponent("Camera"), jsh::CameraComponent(), jsh::LightComponent());
	jshScene::CreateEntity(jsh::TransformComponent(), jsh::LightComponent());

	jsh::CameraComponent* camera = jshScene::GetComponent<jsh::CameraComponent>(cameraEntity);
	camera->SetPerspectiveMatrix(70.f, 5.f, 2000.f);
	jshRenderer::SetCamera(camera);

}


void State::Update(float dt)
{
	jsh::CameraComponent* camera = jshRenderer::GetMainCamera();

	uint8 front = 0u;
	uint8 right = 0u;
	float direction = camera->yaw;

	if (jshInput::IsKey('W')) {
		front = 1;
	}
	if (jshInput::IsKey('S')) {
		if (front) front = 0;
		else front = 2;
	}
	if (jshInput::IsKey('D')) {
		right = 1;
	}
	if (jshInput::IsKey('A')) {
		if (right) right = 0;
		else right = 2;
	}

	if (front || right) {
		if (front == 1) {
			if (right == 1) direction += 45u;
			else if (right == 2) direction -= 45u;
		}
		else if (front == 2) {
			if (right == 1) direction += 135u;
			else if (right == 2) direction -= 135u;
			else direction += 180u;
		}
		else {
			if (right == 1) direction += 90u;
			else direction -= 90u;
		}

		constexpr float force = 15.f;
		jsh::vec2 forward(sin(ToRadians(direction)), cos(ToRadians(direction)));
		forward.Normalize();
		forward *= force * dt;
		camera->position.x += forward.x;
		camera->position.z += forward.y;
	}

	static bool actived = false;
	if (jshInput::IsKeyPressed('C')) actived = !actived;
	jshEvent::Register<jsh::MouseDraggedEvent>(JSH_EVENT_LAYER_DEFAULT, [this, camera, dt](jsh::MouseDraggedEvent& e) {
		if (actived) {
			camera->yaw += ((float)e.draggedX / (float)jshWindow::GetWidth()) * 25000.f * dt;
			camera->pitch += ((float)e.draggedY / (float)jshWindow::GetHeight()) * 16000.f * dt;
		}

		return false;
	});

}

void State::Render()
{
	jsh::MeshComponent* meshComp = jshScene::GetComponent<jsh::MeshComponent>(1);
}

void State::Close()
{

}