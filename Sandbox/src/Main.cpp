#include "jshEngine.h"

struct State : public jsh::State {

	jsh::Model* model;
	jsh::Texture texture;

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
	jshGraphics::SetClearScreenColor(0.1f, 0.4f, 0.9f);
	m_Scene.Create();

	model = jshLoader::LoadModel("res/models/jose.obj", "Pene");
	jshLoader::LoadModel("res/models/dragon.obj", "Dragon");

	jsh::Mesh mesh = jshGraphics::GetModel("Dragon")->meshes[0];

	texture = jshLoader::LoadTexture("res/models/pene.jpg");

	mesh.diffuseMap = texture;

	m_Scene.CreateEntity(jsh::NameComponent("Pene"), jsh::Transform3DComponent(5.f, -5.f, 20.f), jsh::ModelComponent(model));
	m_Scene.CreateEntity(jsh::NameComponent("Dragon"), jsh::Transform3DComponent(-5.f, -5.f, 20.f), jsh::MeshComponent(mesh));
	jsh::Entity cameraEntity = m_Scene.CreateEntity(jsh::NameComponent("Camera"), jsh::CameraComponent());

	jsh::CameraComponent* camera = m_Scene.GetComponent<jsh::CameraComponent>(cameraEntity);
	camera->SetPerspectiveMatrix(70.f, 5.f, 2000.f);
	jshRenderer::SetCamera(camera);

	jsh::Texture skyboxTex = jshLoader::LoadTexture("res/textures/Skybox.png");
	jsh::Skybox skybox;
	skybox.SetTexture(skyboxTex);
	jshRenderer::SetSkybox(skybox);

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