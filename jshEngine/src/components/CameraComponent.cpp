#include "CameraComponent.h"

#include "..//graphics/Window.h"

namespace jsh {

	CameraComponent::CameraComponent() : m_ProjectionMatrix(), m_ViewMatrix(), 
		m_Position(), m_Rotation(),
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
		}
		m_ViewMatrix = XMMatrixTranspose(XMMatrixTranslation(-m_Position.x, -m_Position.y, -m_Position.z)*XMMatrixRotationX(ToRadians(-m_Rotation.x)) * XMMatrixRotationY(ToRadians(-m_Rotation.y)) * XMMatrixRotationZ(ToRadians(-m_Rotation.z)) );
	}

}