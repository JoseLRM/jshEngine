#include "common.h"
#include "GuiComponent.h"

namespace jsh {

	GuiComponent::GuiComponent()
	{
		m_ConstraintTypes[0] = JSH_GUI_CONSTRAINT_TYPE_RELATIVE;
		m_ConstraintTypes[1] = JSH_GUI_CONSTRAINT_TYPE_RELATIVE;
		m_ConstraintTypes[2] = JSH_GUI_CONSTRAINT_TYPE_RELATIVE;
		m_ConstraintTypes[3] = JSH_GUI_CONSTRAINT_TYPE_RELATIVE;
	}

	void GuiComponent::SetConstraintX(JSH_GUI_CONSTRAINT_TYPE type, float value) noexcept
	{
		m_ConstraintTypes[0] = type;
		m_ConstraintValues[0] = value;
		m_Modified = true;
	}
	void GuiComponent::SetConstraintY(JSH_GUI_CONSTRAINT_TYPE type, float value) noexcept
	{
		m_ConstraintTypes[1] = type;
		m_ConstraintValues[1] = value;
		m_Modified = true;
	}
	void GuiComponent::SetConstraintWidth(JSH_GUI_CONSTRAINT_TYPE type, float value) noexcept
	{
		m_ConstraintTypes[2] = type;
		m_ConstraintValues[2] = value;
		m_Modified = true;
	}
	void GuiComponent::SetConstraintHeight(JSH_GUI_CONSTRAINT_TYPE type, float value) noexcept
	{
		m_ConstraintTypes[3] = type;
		m_ConstraintValues[3] = value;
		m_Modified = true;
	}

	void GuiComponent::SetLeftCorner() noexcept
	{
		m_CornerX = 0u;
		m_Modified = true;
	}
	void GuiComponent::SetRightCorner() noexcept
	{
		m_CornerX = 1u;
		m_Modified = true;
	}
	void GuiComponent::SetCentredCornerX() noexcept
	{
		m_CornerX = 2u;
		m_Modified = true;
	}
	void GuiComponent::SetBottomCorner() noexcept
	{
		m_CornerY = 0u;
		m_Modified = true;
	}
	void GuiComponent::SetTopCorner() noexcept
	{
		m_CornerY = 1u;
		m_Modified = true;
	}
	void GuiComponent::SetCentredCornerY() noexcept
	{
		m_CornerY = 2u;
		m_Modified = true;
	}

	void GuiComponent::ReverseX() noexcept
	{
		m_ReverseCoordX = !m_ReverseCoordX;
		m_Modified = true;
	}
	void GuiComponent::ReverseY() noexcept
	{
		m_ReverseCoordY = !m_ReverseCoordY;
		m_Modified = true;
	}


#ifdef JSH_IMGUI
	void GuiComponent::ShowInfo()
	{
		const char* constraintTypeName[4];

		for (uint32 i = 0; i < 4; ++i) {

			switch (m_ConstraintTypes[i])
			{
			case JSH_GUI_CONSTRAINT_TYPE_RELATIVE:
				constraintTypeName[i] = "Relative";
				break;
			case JSH_GUI_CONSTRAINT_TYPE_PIXEL:
				constraintTypeName[i] = "Pixel";
				break;
			case JSH_GUI_CONSTRAINT_TYPE_CENTER:
				constraintTypeName[i] = "Center";
				break;
			default:
				constraintTypeName[i] = "Aspect";
				break;
			}

			if (ImGui::BeginCombo((std::string("Constraint Type ") + std::to_string(i)).c_str(), constraintTypeName[i])) {

				if (ImGui::Button("Relative")) m_ConstraintTypes[i] = JSH_GUI_CONSTRAINT_TYPE_RELATIVE;
				if (ImGui::Button("Pixel")) m_ConstraintTypes[i] = JSH_GUI_CONSTRAINT_TYPE_PIXEL;
				if (ImGui::Button("Center")) m_ConstraintTypes[i] = JSH_GUI_CONSTRAINT_TYPE_CENTER;
				if (ImGui::Button("Aspect")) m_ConstraintTypes[i] = JSH_GUI_CONSTRAINT_TYPE_ASPECT;

				ImGui::EndCombo();
			}

			switch (m_ConstraintTypes[i])
			{
			case JSH_GUI_CONSTRAINT_TYPE_RELATIVE:
				ImGui::DragFloat((std::string("Relative Value[") + std::to_string(i) + "]").c_str(), &m_ConstraintValues[i], 0.01f);
				break;
			case JSH_GUI_CONSTRAINT_TYPE_PIXEL:
				ImGui::DragFloat((std::string("Pixels[") + std::to_string(i) + "]").c_str(), &m_ConstraintValues[i], 1.f);
				break;
			case JSH_GUI_CONSTRAINT_TYPE_ASPECT:
				ImGui::DragFloat((std::string("Aspect Value[") + std::to_string(i) + "]").c_str(), &m_ConstraintValues[i], 0.01f);
				break;
			}
		}

		ImGui::Separator();

		if (ImGui::BeginCombo("Select CornerX", std::to_string(m_CornerX).c_str())) {

			if (ImGui::Button("0")) m_CornerX = 0;
			if (ImGui::Button("1")) m_CornerX = 1;
			if (ImGui::Button("2")) m_CornerX = 2;

			ImGui::EndCombo();
		}
		if (ImGui::BeginCombo("Select CornerY", std::to_string(m_CornerY).c_str())) {

			if (ImGui::Button("0")) m_CornerY = 0;
			if (ImGui::Button("1")) m_CornerY = 1;
			if (ImGui::Button("2")) m_CornerY = 2;

			ImGui::EndCombo();
		}

		if (ImGui::Button(m_ReverseCoordX ? "Normal Coord X" : "Reverse Coord X")) m_ReverseCoordX = !m_ReverseCoordX;
		if (ImGui::Button(m_ReverseCoordY ? "Normal Coord Y" : "Reverse Coord Y")) m_ReverseCoordY = !m_ReverseCoordY;

		m_Modified = true;
	}
#endif

}