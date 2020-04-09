#include "jshEngine.h"

struct State : public jsh::State {

	jsh::Model peneModel;
	jsh::Texture texture;

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

void CreateTerrain(jsh::Mesh* mesh) {

	constexpr uint32 resolution = 100;
	constexpr uint32 size = 1000;
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
			position.y = sin((float)x) + cos((float)z);
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

	mesh->rawData = new jsh::RawData();
	mesh->rawData->SetPositionsAndNormals((float*)positions, (float*)normals, cantOfVertices);
	mesh->rawData->SetColors((uint8*)colors);
	mesh->rawData->SetIndices(indices, cantOfIndices);
	mesh->rawData->Create();

}

void State::Initialize()
{
	jshLoader::LoadModel("res/models/jose.obj", &peneModel);

	std::shared_ptr<jsh::Model> dragonModel = std::make_shared<jsh::Model>();
	jshLoader::LoadModel("res/models/fighter/fighter.obj", dragonModel.get());
	jshGraphics::Save("DragonModel", dragonModel);

	jsh::Mesh mesh = ((jsh::Model*)jshGraphics::Get("DragonModel"))->meshes[0];
	jsh::Mesh terrain;
	CreateTerrain(&terrain);

	jshLoader::LoadTexture("res/models/pene.jpg", &texture);

	mesh.diffuseMap = texture;

	jsh::Scene& scene = jshEngine::GetScene();

	scene.CreateEntity(jsh::NameComponent("Dragon"), jsh::TransformComponent(-5.f, -5.f, 20.f), jsh::MeshComponent(mesh));
	scene.CreateSEntity(1, jsh::NameComponent("Pene"), jsh::TransformComponent(5.f, -5.f, 20.f), jsh::MeshComponent(terrain));
	jsh::Entity cameraEntity = scene.CreateEntity(jsh::NameComponent("Camera"), jsh::CameraComponent(), jsh::LightComponent());
	scene.CreateEntities(2, nullptr, jsh::NameComponent("Light"), jsh::TransformComponent(), jsh::LightComponent(), jsh::MeshComponent(peneModel.meshes[0]));

	jsh::CameraComponent* camera = scene.GetComponent<jsh::CameraComponent>(cameraEntity);
	camera->SetPerspectiveMatrix(70.f, 5.f, 2000.f);
	jshRenderer::SetCamera(camera);

}


void State::Update(float dt)
{
	jsh::CameraComponent* camera = jshRenderer::GetMainCamera();

	float dir = 0.f;
	float direction = camera->yaw;

	if (jshInput::IsKey('W')) {
		dir = direction;
	}
	if (jshInput::IsKey('S')) {
		dir = direction + 180;
	}
	if (jshInput::IsKey('D')) {
		dir = direction + 90;
	}
	if (jshInput::IsKey('A')) {
		dir = direction - 90;
	}

	if (dir != 0.f) {
		constexpr float force = 15.f;
		jsh::vec2 forward(sin(ToRadians(dir)), cos(ToRadians(dir)));
		forward.Normalize();
		forward *= force * dt;
		camera->position.x += forward.x;
		camera->position.z += forward.y;
	}

	static bool actived = false;
	if (jshInput::IsKeyPressed('C')) actived = !actived;
	jshEvent::Register<jsh::MouseDraggedEvent>(JSH_EVENT_LAYER_DEFAULT, [this, camera](jsh::MouseDraggedEvent& e) {
		if (actived) {
			camera->yaw += (e.draggedX / (float)jshWindow::GetWidth()) * 360.f;
			camera->pitch += (e.draggedY / (float)jshWindow::GetHeight()) * 200.f;
		}

		return false;
	});

}

void State::Render()
{
}

void State::Close()
{

}