#pragma once

#include "common.h"
#include "Camera.h"

namespace jsh {

	struct CameraComponent : public Component<CameraComponent> {
	private:
		Camera m_Camera;

	public:
		CameraComponent();

		void UpdateMatrices();

		inline Camera& GetCamera() noexcept { return m_Camera; }
		void UpdateFirstPerson3D(float hSensibility, float vSensibility, float hSpeed, float vSpeed, float dt) noexcept;
		void UpdateFreeCamera2D(float hSpeed, float vSpeed, float zoomSpeed, float dt) noexcept;

#ifdef  JSH_IMGUI
		void ShowInfo() override;
#endif 

	};
	jshDefineComponent(CameraComponent);

}