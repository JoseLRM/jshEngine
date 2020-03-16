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

	constexpr size_t cant = 100000;
	Test* tests = new Test[cant];

	jshTimer::Capture();

	jshTask::Async(cant, jshTask::ThreadCount(), [&tests](jshTask::ThreadArgs& args) {
		tests[args.index].Compute(args.index);
	});

	jshTask::Wait();

	jsh::Time t0 = jshTimer::Capture();

	for (int i = 0; i < cant; ++i) {
		tests[i].Compute(i);
	}

	jsh::Time t1 = jshTimer::Capture();

	jshLogI("With Asynk: %f seconds", t0.GetSecondsf());
	jshLogI("Linear loop: %f seconds", t1.GetSecondsf());
}

void State::Update(float deltaTime)
{
	
}

void State::FixedUpdate()
{
	jsh::Time time = jshTimer::Now();

	jshLogln("Actual time = %f", time.GetSecondsf());
}

void State::Render()
{
	
}

void State::Close()
{

}