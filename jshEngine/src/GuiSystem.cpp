#include "common.h"
#include "GuiSystem.h"

#include "GraphicsAdapter.h"
#include "Input.h"
#include "EventSystem.h"

namespace jsh {

	bool Collide(Transform& trans, vec2 point)
	{
		jsh::vec3 pos = trans.GetWorldPosition();
		jsh::vec3 scl = trans.GetWorldScale();

		float x = pos.x - (scl.x / 2.f);
		float y = pos.y - (scl.y / 2.f);
		float w = scl.x;
		float h = scl.y;

		return x <= point.x && x + w >= point.x && y <= point.y && y + h >= point.y;
	}

	void GuiSystem::Initialize()
	{
		jshEvent::Register<MouseButtonEvent>(JSH_EVENT_LAYER_GUI, [this](MouseButtonEvent& e) {

			std::vector<GuiEventComponent*> toCall;
			vec2 mouse = jshInput::MousePos();
			mouse *= 2.f;
			auto& canvasList = jshScene::_internal::GetComponentsList()[GuiCanvasComponent::ID];

			for (uint32 i = 0; i < canvasList.size(); i += uint32(GuiCanvasComponent::SIZE)) {
				GuiCanvasComponent& canvas = *reinterpret_cast<GuiCanvasComponent*>(&canvasList[i]);

				uint32 childsCount = 0u;
				jsh::Entity* childs;
				jshScene::GetEntitySons(canvas.entity, &childs, &childsCount);

				for (uint32 j = 0; j < childsCount; ++j) {
					jsh::Transform& trans = jshScene::GetTransform(childs[j]);

					// contact with mouse?
					if (!Collide(trans, mouse)) {
						continue;
					}

					if (childs[j] != m_FocusedEntity) {
						UpdateFocusEntity(JSH_EVENT_RELEASED);
					}
					m_FocusedEntity = childs[j];
					UpdateFocusEntity(JSH_EVENT_PRESSED);
					GuiEventComponent* clickedComp = jshScene::GetComponent<GuiEventComponent>(childs[j]);

					if (clickedComp) {
						toCall.push_back(clickedComp);
					}
				}
			}

			// call
			for (int32 i = int32(toCall.size()) - 1; i >= 0; --i) {
				toCall[i]->OnClickedFn(toCall[i]->entity, e);
				if (e.IsDead()) break;
			}

			return true;
		});
	}

	void GuiSystem::Update(float dt)
	{
		auto& canvasList = jshScene::_internal::GetComponentsList()[GuiCanvasComponent::ID];

		// Update Constraints
		for (uint32 i = 0; i < canvasList.size(); i += uint32(GuiCanvasComponent::SIZE)) {
			GuiCanvasComponent& canvas = *reinterpret_cast<GuiCanvasComponent*>(&canvasList[i]);

			uint32 childsCount = 0u;
			jsh::Entity* childs;
			jshScene::GetEntitySons(canvas.entity, &childs, &childsCount);

			uint32 j = 0;
			while (j < childsCount) {
				GuiComponent* gui = jshScene::GetComponent<GuiComponent>(childs[j]);

				if (gui && gui->m_Modified) {
					UpdateGui(*gui, canvas);
					
					uint32 guiChilds = 0u;
					jshScene::GetEntitySons(gui->entity, nullptr, &guiChilds);

					j++;
					uint32 w = 0u;
					while (w < guiChilds) {
						GuiComponent* guiChild = jshScene::GetComponent<GuiComponent>(childs[j + w]);
						if (guiChild) {
							UpdateGui(*guiChild, canvas);
						}

						++w;
					}

					j += guiChilds;
				}
				else {
					++j;
				}
			}
		}

		// Focus Event
		UpdateFocusEntity(0);

		// Contact Events
		vec2 mouse = jshInput::MousePos();
		mouse *= 2.f;

		for (uint32 i = 0; i < canvasList.size(); i += uint32(GuiCanvasComponent::SIZE)) {
			GuiCanvasComponent& canvas = *reinterpret_cast<GuiCanvasComponent*>(&canvasList[i]);

			uint32 childsCount = 0u;
			jsh::Entity* childs;
			jshScene::GetEntitySons(canvas.entity, &childs, &childsCount);

			for (uint32 j = 0; j < childsCount; ++j) {
				jsh::Transform& trans = jshScene::GetTransform(childs[j]);

				// contact with mouse?
				if (!Collide(trans, mouse)) {
					uint32 guiChilds = 0u;
					jshScene::GetEntitySons(childs[j], nullptr, &guiChilds);

					uint32 w = 0;
					while (w <= guiChilds) {

						GuiEventComponent* contactComp = jshScene::GetComponent<GuiEventComponent>(childs[j + w]);
						if (contactComp) {
							if (contactComp->m_Pressed) {
								if(contactComp->OnContactFn != nullptr) contactComp->OnContactFn(childs[j], JSH_EVENT_RELEASED);
								contactComp->m_Pressed = false;
							}
							else break;
						}

						++w;
					}

					j += guiChilds;
					continue;
				}

				GuiEventComponent* contactComp = jshScene::GetComponent<GuiEventComponent>(childs[j]);

				if (contactComp) {
					if (!contactComp->m_Pressed && contactComp->OnContactFn != nullptr) {
						contactComp->OnContactFn(childs[j], JSH_EVENT_PRESSED);
					}
					else if (contactComp->OnContactFn != nullptr) contactComp->OnContactFn(childs[j], 0u);
					contactComp->m_Pressed = true;
				}
			}
		}

	}

	void GuiSystem::UpdateGui(GuiComponent& gui, GuiCanvasComponent& canvas)
	{
		Transform& trans = jshScene::GetTransform(gui.entity);

		Entity parent = jshScene::GetEntityParent(gui.entity);
		float aspect = (float)jshGraphics::GetResolutionWidth() / (float)jshGraphics::GetResolutionHeight();
		if (parent != INVALID_ENTITY) {
			jsh::vec3 scale = jshScene::GetTransform(parent).GetWorldScale();
			aspect *= scale.x / scale.y;
		}

		vec4 v;

		v.z = CalculateDimension(gui.m_ConstraintTypes[2], gui.m_ConstraintValues[2], 0.f, 0u);
		v.w = CalculateDimension(gui.m_ConstraintTypes[3], gui.m_ConstraintValues[3], v.z * aspect, 1u);
		v.x = CalculateCoord(gui.m_ConstraintTypes[0], gui.m_ConstraintValues[0], 0.f, v.z, 0u);
		v.y = CalculateCoord(gui.m_ConstraintTypes[1], gui.m_ConstraintValues[1], v.x * aspect, v.w, 1u);

		aspect = 1.f / aspect;

		if (gui.m_ConstraintTypes[0] == JSH_GUI_CONSTRAINT_TYPE_ASPECT || gui.m_ConstraintTypes[2] == JSH_GUI_CONSTRAINT_TYPE_ASPECT) {
			v.z = CalculateDimension(gui.m_ConstraintTypes[2], gui.m_ConstraintValues[2], v.w * aspect, 0u);
			v.x = CalculateCoord(gui.m_ConstraintTypes[0], gui.m_ConstraintValues[0], v.y * aspect, v.z, 0u);
		}

		AdjustCoord(gui.m_CornerX, gui.m_ReverseCoordX, v.x, v.z);
		AdjustCoord(gui.m_CornerY, gui.m_ReverseCoordY, v.y, v.w);
		
		// Ajusting positions and dimension in parent space
		v.x -= 0.5f;
		v.y -= 0.5f;

		// Ajusting positions and dimension in screen space
		if (parent == canvas.entity) {
			v.x = (v.x * 2.f);
			v.y = (v.y * 2.f);
			v.z = (v.z * 2.f);
			v.w = (v.w * 2.f);
		}

		trans.SetPosition({ v.x, v.y, 0.f });
		trans.SetScale({ v.z, v.w, 0.f });

		gui.m_Modified = false;
	}

	void GuiSystem::UpdateFocusEntity(uint8 mode)
	{
		if (m_FocusedEntity != INVALID_ENTITY) {
			GuiEventComponent* comp = jshScene::GetComponent<GuiEventComponent>(m_FocusedEntity);
			if (comp && comp->OnFocusFn != nullptr) {
				bool lostFocus = false;
				comp->OnFocusFn(m_FocusedEntity, mode, lostFocus);
				if (lostFocus) m_FocusedEntity = INVALID_ENTITY;
			}
		}
	}

	float GuiSystem::CalculateCoord(JSH_GUI_CONSTRAINT_TYPE type, float value, float aspectValue, float dimensionValue, uint8 coordsType)
	{
		float coord = 0.f;

		switch (type)
		{
		case JSH_GUI_CONSTRAINT_TYPE_RELATIVE:
			coord = value;
			break;
		case JSH_GUI_CONSTRAINT_TYPE_PIXEL:
		{
			float res = float(coordsType == 1 ? jshGraphics::GetResolutionHeight() : jshGraphics::GetResolutionWidth());
			coord = value / res;
		}
			break;
		case JSH_GUI_CONSTRAINT_TYPE_CENTER:
			coord = 0.5f;
			break;
		case JSH_GUI_CONSTRAINT_TYPE_ASPECT:
		{
			coord = aspectValue * value;
		}
			break;
		}

		return coord;
	}
	float GuiSystem::CalculateDimension(JSH_GUI_CONSTRAINT_TYPE type, float value, float aspectValue, uint8 dimensionType)
	{
		float dimension = 0.f;

		switch (type)
		{
		case JSH_GUI_CONSTRAINT_TYPE_RELATIVE:
			dimension = value;
			break;
		case JSH_GUI_CONSTRAINT_TYPE_PIXEL:
		{
			float res = float(dimensionType == 1 ? jshGraphics::GetResolutionHeight() : jshGraphics::GetResolutionWidth());
			dimension = value / res;
		}
			break;
		case JSH_GUI_CONSTRAINT_TYPE_CENTER:
			dimension = 0.5f;
			break;
		case JSH_GUI_CONSTRAINT_TYPE_ASPECT:
		{
			dimension = aspectValue * value;
			break;
		}
		}

		return dimension;
	}

	void GuiSystem::AdjustCoord(uint8 corner, bool reversed, float& coord, float dimension)
	{
		switch (corner)
		{
		case 0: // left - bottom
			coord += dimension / 2.f;
			if(reversed) coord = (1.f - coord);
			break;
		case 1: // right - top
			coord -= dimension / 2.f;
			if (reversed) coord = (1.f - coord);
			break;
		case 2: // centred
			if (reversed) coord = (1.f - coord);
			break;
		}
	}

}