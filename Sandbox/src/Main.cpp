#include "jshEngine.h"

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

State::State() {}

void State::Initialize()
{
	jshGraphics::SetTopology(JSH_TOPOLOGY_TRIANGLES);
	jshGraphics::SetClearScreenColor(0.1f, 0.4f, 0.9f);

	
	jshEvent::Register<jsh::MouseEvent>(JSH_EVENT_LAYER_DEFAULT, [](jsh::MouseEvent& e) {
		
		if (e.IsButton()) {
			jshLogln("Mouse pressed");
		}
		else if (e.IsDragged()) {
			jshLogln("Mouse dragged");
		}
		else if(e.IsWheel()){
			jshLogln("Mouse wheel");
		}

		return false;
	});
	
	/*
	jshEvent::Register<jsh::MouseButtonReleasedEvent>(JSH_EVENT_LAYER_DEFAULT, [](jsh::MouseButtonReleasedEvent& e) {

		jshLogln("Mouse pressed %u", e.buttonCode);

		return true;
	});
	*/
	
	jshEvent::Register<jsh::MouseDraggedEvent>(JSH_EVENT_LAYER_SYSTEM, [](jsh::MouseDraggedEvent& e) {
	
		if (e.draggedX < 0) e.Kill();
	
		return true;
	});
	

	jshEvent::Register<jsh::MouseDraggedEvent>(JSH_EVENT_LAYER_DEFAULT, [](jsh::MouseDraggedEvent& e) {

		jshLogln("Mouse dragged, pos(%u, %u), dragged(%i, %i)", e.mouseX, e.mouseY, e.draggedX, e.draggedY);

		return true;
	});
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