#pragma once

#include "Scene.h"

enum JSH_GUI_CONSTRAINT_TYPE : uint8 {
	JSH_GUI_CONSTRAINT_TYPE_RELATIVE,
	JSH_GUI_CONSTRAINT_TYPE_PIXEL,
	JSH_GUI_CONSTRAINT_TYPE_CENTER,
	JSH_GUI_CONSTRAINT_TYPE_ASPECT,
};

namespace jsh {

	class GuiSystem;

	struct GuiComponent : public Component<GuiComponent> {
	private:
		JSH_GUI_CONSTRAINT_TYPE m_ConstraintTypes[4];
		float m_ConstraintValues[4] = {0.f, 0.f, 0.1f, 0.1f};

		/*
		0 -> left
		1 -> right
		2 -> center 
		*/
		uint8 m_CornerX = 2u;
		/*
		0 -> bottom
		1 -> top
		2 -> center
		*/
		uint8 m_CornerY = 2u;
		bool m_ReverseCoordX = false;
		bool m_ReverseCoordY = false;

		bool m_Modified = true;

	public:
		friend GuiSystem;
		GuiComponent();

		// setters
		void SetConstraintX(JSH_GUI_CONSTRAINT_TYPE type, float value = 0.f) noexcept;
		void SetConstraintY(JSH_GUI_CONSTRAINT_TYPE type, float value = 0.f) noexcept;
		void SetConstraintWidth(JSH_GUI_CONSTRAINT_TYPE type, float value = 0.f) noexcept;
		void SetConstraintHeight(JSH_GUI_CONSTRAINT_TYPE type, float value = 0.f) noexcept;

		void SetLeftCorner() noexcept;
		void SetRightCorner() noexcept;
		void SetCentredCornerX() noexcept;
		void SetBottomCorner() noexcept;
		void SetTopCorner() noexcept;
		void SetCentredCornerY() noexcept;

		void ReverseX() noexcept;
		void ReverseY() noexcept;

		// getters
		inline JSH_GUI_CONSTRAINT_TYPE GetConstraintX() const noexcept { return m_ConstraintTypes[0]; }
		inline JSH_GUI_CONSTRAINT_TYPE GetConstraintY() const noexcept { return m_ConstraintTypes[1]; }
		inline JSH_GUI_CONSTRAINT_TYPE GetConstraintWidth() const noexcept { return m_ConstraintTypes[2]; }
		inline JSH_GUI_CONSTRAINT_TYPE GetConstraintHeight() const noexcept { return m_ConstraintTypes[3]; }

		inline float GetConstraintValueX() const noexcept { return m_ConstraintValues[0]; }
		inline float GetConstraintValueY() const noexcept { return m_ConstraintValues[1]; }
		inline float GetConstraintValueWidth() const noexcept { return m_ConstraintValues[2]; }
		inline float GetConstraintValueHeight() const noexcept { return m_ConstraintValues[3]; }

		inline bool IsLeftCorner() const noexcept { return m_CornerX == 0; }
		inline bool IsRightCorner() const noexcept { return m_CornerX == 1; }
		inline bool IsCentredCornerX() const noexcept { return m_CornerX == 2; }

		inline bool IsBottomCorner() const noexcept { return m_CornerY == 0; }
		inline bool IsTopCorner() const noexcept { return m_CornerY == 1; }
		inline bool IsCentredCornerY() const noexcept { return m_CornerY == 2; }

		inline bool IsReversedX() const noexcept { return m_ReverseCoordX; }
		inline bool IsReversedY() const noexcept { return m_ReverseCoordY; }

#ifdef JSH_IMGUI
		void ShowInfo() override;
#endif

	};
	jshDefineComponent(GuiComponent);

}