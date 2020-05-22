#pragma once

#include "GuiComponent.h"
#include "GuiCanvasComponent.h"
#include "GuiEventComponent.h"

namespace jsh {

	class GuiSystem {
		jsh::Entity m_FocusedEntity = INVALID_ENTITY;

		jshEvent::Listener<jsh::MouseButtonEvent> m_MouseListener;
		jshEvent::Listener<jsh::ResolutionEvent> m_ResolutionListener;

	public:
		void Initialize();
		void Update(float dt);

	private:
		void UpdateGui(GuiComponent& gui, GuiCanvasComponent& canvas);
		void UpdateFocusEntity(uint8 mode);

		float CalculateCoord(JSH_GUI_CONSTRAINT_TYPE type, float value, float aspectValue, float dimensionValue, uint8 coordType);
		float CalculateDimension(JSH_GUI_CONSTRAINT_TYPE type, float value, float aspectValue, uint8 dimensionType);
		void AdjustCoord(uint8 corner, bool reversed, float& coord, float dimension);

	};

}