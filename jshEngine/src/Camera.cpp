#include "common.h"

#include "Camera.h"
#include "Graphics.h"
#include "Renderer.h"

namespace jsh {

	Camera::Camera()
	{
		m_Aspect = (float)jshGraphics::GetResolution().x / (float)jshGraphics::GetResolution().y;
	}

	void Camera::SetDimension(float dimension) noexcept
	{
		m_Dimension = dimension;
	}
	void Camera::SetFieldOfView(float fov) noexcept
	{
		m_Dimension = tan(fov / 2.f) * m_Near;
	}

	void Camera::SetNear(float near) noexcept
	{
		m_Near = near;
	}
	void Camera::SetFar(float far) noexcept { m_Far = far; }

	void Camera::UpdateMatrices() noexcept
	{
		if (m_AttachResolutionAspect)
		{
			m_Aspect = jshGraphics::GetResolutionAspect();
		}

		if (Is3D()) {
			UpdateMatrices3D();
		}
		else {
			UpdateMatrices2D();
		}

		// Update Mouse Position
		m_MousePosition = jshInput::MousePos() * vec2(m_Dimension, m_Dimension / m_Aspect) + vec2(position.x, position.y);

		// Update Mouse Ray Direction
		XMVECTOR ray = XMVectorSet(jshInput::MousePos().x * 2.f, jshInput::MousePos().y * 2.f, 1.f, 1.f);
		XMMATRIX InversedViewProjection = XMMatrixInverse(nullptr, m_ViewMatrix * m_ProjectionMatrix);
		ray = XMVector4Transform(ray, InversedViewProjection);
		ray = XMVector3Normalize(ray);
		m_MouseRay = ray;
	}

	void Camera::UpdateMatrices2D() noexcept
	{
		if (IsOrthographic()) {
			m_ProjectionMatrix = XMMatrixOrthographicLH(m_Dimension, m_Dimension / m_Aspect, -1000.f, 1000.f);
			m_ViewMatrix = XMMatrixScaling(scale2D.x, scale2D.y, 1.f) * XMMatrixRotationZ(rotation.z) * XMMatrixTranslation(-position.x, -position.y, 0.f);
		}
	}
	void Camera::UpdateMatrices3D() noexcept
	{
		if (IsOrthographic()) {
			m_ProjectionMatrix = XMMatrixOrthographicLH(m_Dimension, m_Dimension / m_Aspect, m_Near, m_Far);
		}
		else {
			if (m_Near <= 0.f) m_Near = 0.001f;
			if (m_Far <= 0.f) m_Far = 0.001f;

			m_ProjectionMatrix = XMMatrixPerspectiveLH(m_Dimension, m_Dimension / m_Aspect, m_Near, m_Far);
		}

		XMVECTOR direction = XMVectorSet(0.f, 0.f, 1.f, 0.f);

		if (abs(ToDegrees(rotation.x)) >= 90.f) rotation.x = ToRadians((ToDegrees(rotation.x) > 0.f ? 1.f : -1.f) * 89.9f);

		direction = XMVector3Transform(direction, XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, 0.f));

		const auto target = XMVECTOR(position) + direction;
		m_ViewMatrix = XMMatrixLookAtLH(position, target, XMVectorSet(0.f, 1.f, 0.f, 0.f));
	}

#ifdef  JSH_IMGUI
	void Camera::ShowImGuiWindow()
	{
		ImGui::DragFloat("Dimension", &m_Dimension, 0.1f);
		ImGui::DragFloat("Aspect", &m_Aspect, 0.001f, 0.01f, 0.99f);
		float fov = ToDegrees(GetFieldOfView());
		if (ImGui::SliderFloat("Fov", &fov, 0.01f, 179.9f)) {
			SetFieldOfView(ToRadians(fov));
		}
		ImGui::DragFloat("Near", &m_Near, 0.05f, 0.01f, FLT_MAX);
		ImGui::DragFloat("Far", &m_Far, 1.f, 0.01f, FLT_MAX);

		if (ImGui::Button(m_Orthographic ? "Change to Perspective" : "Change to Orthographic")) {
			m_Orthographic = !m_Orthographic;
		}
		if (ImGui::Button(m_3D ? "Set 2D" : "Set 3D")) {
			m_3D = !m_3D;
		}

		ImGui::Checkbox("Attach To Resolution", &m_AttachResolutionAspect);
	}

#endif 


}