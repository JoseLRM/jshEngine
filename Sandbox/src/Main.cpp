#include "jshEngine.h"

struct TestComponent : jsh::Component<TestComponent> {
	int data;
	TestComponent() : data(0) {}
	TestComponent(int i) : data(i) {}

	jshImGui(void ShowInfo() override {

		ImGui::Text("Test Component");
		ImGui::DragInt("Data", &data, 1);

	});
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

		testComponent->data += sin(testComponent->data) * (int)103.f;
		for (uint32 i = 0; i < 1; ++i) {
			testComponent->data *= (i % 2 == 0) ? 8 : (1.f / 8.f);
		}
	}

	void UpdateEntities(jsh::Scene& scene, jsh::vector<jsh::BaseComponent**>& components, float dt) override
	{
		for (uint32 i = 0; i < components.size(); ++i) {
			jsh::BaseComponent** comp = components[i];
			TestComponent* testComponent = (TestComponent*)comp[0];
			jsh::Entity entity = testComponent->entityID;
			HelloTag* tag0 = (HelloTag*)comp[1];
			PeneTag* tag1 = (PeneTag*)comp[2];

			testComponent->data += sin(testComponent->data) * (int)103.f;
		}
	}

};

class State : public jsh::State
{
	jsh::Buffer m_VertexBuffer;
	jsh::Buffer m_IndexBuffer;

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

	//system0.SetCollectiveSystem();
	system0.SetExecuteType(JSH_ECS_SYSTEM_PARALLEL);
	system1.SetExecuteType(JSH_ECS_SYSTEM_PARALLEL);
	system2.SetExecuteType(JSH_ECS_SYSTEM_PARALLEL);
	system3.SetExecuteType(JSH_ECS_SYSTEM_PARALLEL);
	system4.SetExecuteType(JSH_ECS_SYSTEM_PARALLEL);

	//m_Scene.CreateEntities(10000, nullptr, TestComponent(69));
	
}

void State::Update(float deltaTime)
{
	jsh::System* systems[] = {
		&system0,
		&system1,
		&system2,
		&system3,
		&system4,
	};
	jsh::Time bTime = jshTimer::Now();
	m_Scene.UpdateSystems(systems, 5, deltaTime);
	//jsh::Time time = jshTimer::Now() - bTime;
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