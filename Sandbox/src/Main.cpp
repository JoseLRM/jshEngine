#include "jshEngine.h"

#include <chrono>

class State : public jsh::State
{
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

State::State()
{
}

class Test {
	float m[16];
public:
	void Compute(uint32 n) {
		for (int i = 0; i < 16; ++i) {
			m[i] = i * n + n;
			std::vector<float> inutil;
			for (int j = 0; j < 16; j++) inutil.push_back(m[j]);

			std::sort(inutil.begin(), inutil.end());
		}
	}
};

void State::Initialize()
{
	jshGraphics::SetTopology(JSH_TOPOLOGY_TRIANGLES);
	jshGraphics::SetClearScreenColor(0.1f, 0.4f, 0.9f);

	
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

}