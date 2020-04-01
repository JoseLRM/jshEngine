#include "CameraComponent.h"

#include "..//graphics/Window.h"

namespace jsh {

	CameraComponent::CameraComponent() : m_ProjectionMatrix(), m_ViewMatrix(), 
		m_Position(), m_Pitch(0.f), m_Yaw(0.f),
		m_Width(100.f), m_Height(100.f), m_Fov(70.f), m_Near(5.f), m_Far(2000.f) {
		SetPerspectiveMatrix(m_Fov, m_Near, m_Far);
	}

	void CameraComponent::SetPerspectiveMatrix(float fov, float near, float far) noexcept
	{
		m_ProjectionMatrix = XMMatrixTranspose(XMMatrixPerspectiveFovLH(ToRadians(fov), GetAspectRatio(), near, far));
		m_Fov = fov;
		m_Near = near;
		m_Far = far;
	}
	void CameraComponent::SetOrthographicMatrix(float width, float height) noexcept
	{
		m_ProjectionMatrix = XMMatrixTranspose(XMMatrixOrthographicLH(width, height, 0.f, 1.f));
		m_Width = width;
		m_Height = height;
	}

	float CameraComponent::GetAspectRatio() const noexcept
	{
		return (float)jshWindow::GetWidth() / (float)jshWindow::GetHeight();
	}

	void CameraComponent::UpdateMatrices() noexcept
	{
		if (IsOrthographic()) {
			SetOrthographicMatrix(m_Width, m_Height);
		}
		else {
			SetPerspectiveMatrix(m_Fov, m_Near, m_Far);

			XMVECTOR direction = XMVectorSet(0.f, 0.f, 1.f, 0.f);

			if (abs(m_Pitch) >= 90.f) m_Pitch = (m_Pitch > 0.f ? 1.f : -1.f) * 89.9f;

			direction = XMVector3Transform(direction, XMMatrixRotationRollPitchYaw(ToRadians(m_Pitch), ToRadians(m_Yaw), ToRadians(0.f)));

			const auto target = XMVECTOR(m_Position) + direction;
			m_ViewMatrix = XMMatrixTranspose(XMMatrixLookAtLH(m_Position, target, XMVectorSet(0.f, 1.f, 0.f, 0.f)));

		}
	}

}