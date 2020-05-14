#pragma once

#include "Scene.h"

namespace jsh {

	class GuiSystem;
	struct MouseButtonEvent;

	struct GuiEventComponent : public Component<GuiEventComponent> {
	private:
		bool m_Pressed = false;

	public:
		friend GuiSystem;

		std::function<void(Entity, MouseButtonEvent&)>	OnClickedFn;
		std::function<void(Entity, uint8 mode)>			OnContactFn;
		std::function<void(Entity, uint8 mode, bool&)>	OnFocusFn;

	};
	jshDefineComponent(GuiEventComponent);

}