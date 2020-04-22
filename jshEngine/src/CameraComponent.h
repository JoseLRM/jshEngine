#pragma once

#include "Scene.h"

namespace jsh {

	class CameraComponent : public Component<CameraComponent> {

		XMMATRIX m_ProjectionMatrix;
		XMMATRIX m_ViewMatrix;

		bool m_IsOrthographic = false;

		// orthographic projection
		float m_Width;
		float m_Height;

		// perspective projection
		float m_Fov;
		float m_Near;
		float m_Far;

	public:
		CameraComponent();

		void UpdateFirstPerson(float hSensibility, float vSensibility, float hSpeed, float vSpeed, float dt) noexcept;

		void SetPerspectiveMatrix(float fov, float near, float far) noexcept;
		void SetOrthographicMatrix(float width, float height) noexcept;

		void UpdateMatrices() noexcept;

		inline bool IsOrthographic() const noexcept { return m_IsOrthographic; }
		inline bool IsPerspective() const noexcept { return !m_IsOrthographic; }

		inline const XMMATRIX& GetProjectionMatrix() const noexcept { return m_ProjectionMatrix; }
		inline const XMMATRIX& GetViewMatrix() const noexcept { return m_ViewMatrix; }

	private:
		float GetAspectRatio() const noexcept;

#ifdef  JSH_IMGUI
	public:
		void ShowInfo() override;
#endif 

	};
	jshDefineComponent(CameraComponent);

}