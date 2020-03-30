#include "jshEngine.h"

struct State : public jsh::State {

	jsh::Model* model;

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

	model = jshLoader::LoadModel("res/models/jose.obj");
	jsh::Mesh* mesh = jshLoader::LoadModel("res/models/dragon.obj")->meshes[0];

	m_Scene.CreateEntity(jsh::Transform3DComponent(5.f, -5.f, 20.f), jsh::ModelComponent(model));
	m_Scene.CreateEntity(jsh::Transform3DComponent(-5.f, -5.f, 20.f), jsh::MeshComponent(mesh));

}


void State::Update(float dt)
{

}

void State::Render()
{
	jshRenderer3D::DrawScene(&m_Scene);	
}

void State::Close()
{

}