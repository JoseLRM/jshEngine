#include "jshEngine.h"

class State : public jsh::State
{
	jsh::Buffer m_VertexBuffer;
	jsh::Buffer m_IndexBuffer;

public:
	State();

	void Initialize() override;
	void Update(float deltaTime) override;
	void FixedUpdate() override;
	void Render() override;
	void Close() override;
};

template<class vector>
void vectorTest()
{
	vector v;

	v.reserve(100);

	for (int i = 0; i < 100; i++) {
		v.push_back(i * 2);
	}

	int sum = 0;
	for (int i = 1; i < v.size(); i++) {
		sum += v.back();
		v.pop_back();
	}
}

int main()
{
	jshApplication::Initialize(new State());

	int times = 100;

	jsh::Time t0;
	jsh::Time t1;

	for (int j = 0; j < times; j++) {
		jshTimer::Capture();

		vectorTest<std::vector<int>>();

		t0 += jshTimer::Capture();
		
		vectorTest<jsh::vector<int>>();

		t1 += jshTimer::Capture();
	}

	jshLogln("Time std = %f", t0.GetSecondsf() / times);
	jshLogln("Time jsh = %f", t1.GetSecondsf() / times);

	float porcentage = t0 / t1;
	jshLogln("Optimizado un %i%c", (int)(porcentage * 100.f)-100, '%');

	return 0;
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
}

void State::Update(float deltaTime)
{
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