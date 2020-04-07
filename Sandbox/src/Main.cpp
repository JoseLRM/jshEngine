#include "jshEngine.h"

struct State : public jsh::State {

	jsh::Model peneModel;
	jsh::Texture texture;
	float time = 0.f;
	bool day = true;

	void Initialize() override;
	void Update(float dt) override;
	void Render() override;
	void Close() override;

};

int main()
{

	jshApplication::Initialize(new State());
	jshApplication::Run();
	jshApplication::Close();

	return 0;
}

void State::Initialize()
{
	m_Scene.Create();

	jshLoader::LoadModel("res/models/jose.obj", &peneModel);

	std::shared_ptr<jsh::Model> dragonModel = std::make_shared<jsh::Model>();
	jshLoader::LoadModel("res/models/dragon.obj", dragonModel.get());
	jshGraphics::Save("DragonModel", dragonModel);

	jsh::Mesh mesh = ((jsh::Model*)jshGraphics::Get("DragonModel"))->meshes[0];

	jshLoader::LoadTexture("res/models/pene.jpg", &texture);

	mesh.diffuseMap = texture;

	m_Scene.CreateEntity(jsh::NameComponent("Dragon"), jsh::Transform3DComponent(-5.f, -5.f, 20.f), jsh::MeshComponent(mesh));
	m_Scene.CreateEntity(jsh::NameComponent("Pene"), jsh::Transform3DComponent(5.f, -5.f, 20.f), jsh::ModelComponent(&peneModel));
	jsh::Entity cameraEntity = m_Scene.CreateEntity(jsh::NameComponent("Camera"), jsh::CameraComponent());

	jsh::CameraComponent* camera = m_Scene.GetComponent<jsh::CameraComponent>(cameraEntity);
	camera->SetPerspectiveMatrix(70.f, 5.f, 2000.f);
	jshRenderer::SetCamera(camera);

}


void State::Update(float dt)
{
}

void State::Render()
{
	jshRenderer::DrawScene(&m_Scene);	
}

void State::Close()
{

}