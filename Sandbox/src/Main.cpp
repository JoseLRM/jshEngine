#include "jshEngine.h"

struct TestComponent : jsh::Component<TestComponent> {
	int data;
	std::map<int, int> wtf;
	TestComponent() : data(0) {}
	TestComponent(int i) : data(i) {}
};
jshDefineTag(HelloTag);
jshDefineTag(PeneTag);
jshDefineComponent(TestComponent);

struct System : public jsh::System {

	System() : jsh::System()
	{
		AddRequestedComponents<TestComponent>();
		AddOptionalComponents<HelloTag, PeneTag>();
	}

	void UpdateEntity(jsh::Scene& scene, jsh::Entity entity, jsh::BaseComponent** comp, float dt) override
	{
		TestComponent* testComponent = (TestComponent*) comp[0];
		HelloTag* tag0 = (HelloTag*)comp[1];
		PeneTag* tag1 = (PeneTag*)comp[2];

		jshLogln("[%s] Entity %u: Data = %i, Tag0 = %s, Tag1 = %s", GetName(), entity, testComponent->data, tag0 ? "true" : "false", tag1 ? "true" : "false");
	}

	void UpdateEntities(jsh::Scene& scene, jsh::vector<jsh::BaseComponent**>& components, float dt) override
	{
		for (uint32 i = 0; i < components.size(); ++i) {
			jsh::BaseComponent** comp = components[i];
			TestComponent* testComponent = (TestComponent*)comp[0];
			jsh::Entity entity = testComponent->entityID;
			HelloTag* tag0 = (HelloTag*)comp[1];
			PeneTag* tag1 = (PeneTag*)comp[2];

			jshLogln("[%s col] Entity %u: Data = %i, Tag0 = %s, Tag1 = %s", GetName(), entity, testComponent->data, tag0 ? "true" : "false", tag1 ? "true" : "false");
		}
	}

};

class State : public jsh::State
{
	jsh::Buffer m_VertexBuffer;
	jsh::Buffer m_IndexBuffer;

	jsh::Scene m_Scene;
	System system0;
	System system1;
	System system2;
	System system3;
	System system4;

public:
	State();

	void Initialize() override;
	void Update(float deltaTime) override;
	void FixedUpdate() override;
	void Render() override;
	void Close() override;
};

int main()
{
	jshApplication::Initialize(new State());
	jshApplication::Run();

	return 0;
}

State::State() {}

void State::Initialize()
{
	jshGraphics::SetTopology(JSH_TOPOLOGY_TRIANGLES);
	jshGraphics::SetClearScreenColor(0.1f, 0.4f, 0.9f);

	struct Vertex {
		float x, y;
	};

	Vertex vertexData[] = {
		{0.f, 0.5f},
		{0.5f, -0.5f},
		{-0.5f, -0.5f}
	};

	uint32 indexData[] = {
		0, 1, 2
	};

	jshGraphics::CreateVertexBuffer(m_VertexBuffer, &vertexData, sizeof(Vertex) * std::size(vertexData), sizeof(float) * 2);
	jshGraphics::CreateIndexBuffer(m_IndexBuffer, &indexData, sizeof(uint32), std::size(indexData));
	
	m_Scene.Create();

	m_Scene.CreateEntities(100, nullptr, PeneTag());
	jsh::Entity e = m_Scene.CreateEntity(TestComponent(69));
	m_Scene.CreateEntities(50, nullptr, TestComponent(0), HelloTag());
	jsh::vector<jsh::Entity> entities;
	m_Scene.CreateSEntities(e, 50, &entities, TestComponent(1), HelloTag());
	m_Scene.CreateSEntities(entities[5], 50, nullptr, TestComponent(6969), HelloTag());

	system0.SetExecuteType(JSH_ECS_SYSTEM_MULTITHREADED);
	system0.SetTreeMode(true);

	system1.SetExecuteType(JSH_ECS_SYSTEM_PARALLEL);
	system1.SetTreeMode(true);
	system1.SetCollectiveSystem();

	system2.SetExecuteType(JSH_ECS_SYSTEM_PARALLEL);
	system2.SetTreeMode(true);

	system3.SetExecuteType(JSH_ECS_SYSTEM_PARALLEL);
	system3.SetTreeMode(true);

	system4.SetExecuteType(JSH_ECS_SYSTEM_PARALLEL);
	system4.SetTreeMode(true);

}

void State::Update(float deltaTime)
{
	jshLogln("----------------------------");
	jsh::System* systems[] = {
		&system1,
		&system2,
		&system0,
		&system3,
		&system4
	};
	m_Scene.UpdateSystems(systems, 5, deltaTime);
}

void State::FixedUpdate()
{
}

void State::Render()
{
	
}

void State::Close()
{
	m_VertexBuffer.Clear();
	m_IndexBuffer.Clear();
}