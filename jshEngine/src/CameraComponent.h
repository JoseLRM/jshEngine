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
		void ShowInfo() override
		{
			if (m_IsOrthographic) {
				ImGui::DragFloat("Width", &m_Width, 1.f, 0.01f, FLT_MAX);
				ImGui::DragFloat("Height", &m_Height, 1.f, 0.01f, FLT_MAX);
				if (ImGui::Button("Adjust")) {
					m_Height = m_Width / GetAspectRatio();
				}
			}
			else {
				ImGui::DragFloat("Fov", &m_Fov, 0.25f, 0.01f, FLT_MAX);
				ImGui::DragFloat("Near", &m_Near, 0.05f, 0.01f, FLT_MAX);
				ImGui::DragFloat("Far", &m_Far, 1.f, 0.01f, FLT_MAX);
			}
			if (ImGui::Button("Change Projection")) {
				m_IsOrthographic = !m_IsOrthographic;
			}
		}

#endif 

	};
	jshDefineComponent(CameraComponent);

}