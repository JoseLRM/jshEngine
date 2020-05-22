#pragma once

#include "Scene.h"

namespace jsh {

	class GuiSystem;

	struct GuiCanvasComponent : Component<GuiCanvasComponent> {
	private:
		bool m_UpdateAll = true;

	public:
		friend GuiSystem;

	};
	jshDefineComponent(GuiCanvasComponent);

}