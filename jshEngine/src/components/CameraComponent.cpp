#include "CameraComponent.h"

#include "..//graphics/Window.h"
#include "..//Input.h"

namespace jsh {

	CameraComponent::CameraComponent() : m_ProjectionMatrix(), m_ViewMatrix(),
		m_Width(100.f), m_Height(100.f), m_Fov(70.f), m_Near(0.1f), m_Far(2000.f) {
		SetPerspectiveMatrix(m_Fov, m_Near, m_Far);
	}

	void CameraComponent::UpdateFirstPerson(float hSensibility, float vSensibility, float hSpeed, float vSpeed, float dt) noexcept
	{

		vSpeed *= dt;
		hSpeed *= dt;

		uint8 front = 0u;
		uint8 right = 0u;
		jsh::Transform& trans = jshScene::GetTransform(entityID);
		float direction = trans.GetLocalRotation().y;
		jsh::vec3 pos = trans.GetLocalPosition();

		if (jshInput::IsKey('W')) {
			front = 1;
		}
		if (jshInput::IsKey('S')) {
			if (front) front = 0;
			else front = 2;
		}
		if (jshInput::IsKey('D')) {
			right = 1;
		}
		if (jshInput::IsKey('A')) {
			if (right) right = 0;
			else right = 2;
		}

		if (front || right) {
			if (front == 1) {
				if (right == 1) direction += 45u;
				else if (right == 2) direction -= 45u;
			}
			else if (front == 2) {
				if (right == 1) direction += 135u;
				else if (right == 2) direction -= 135u;
				else direction += 180u;
			}
			else {
				if (right == 1) direction += 90u;
				else direction -= 90u;
			}

			jsh::vec2 forward(sin(ToRadians(direction)), cos(ToRadians(direction)));
			forward.Normalize();
			forward *= hSpeed;

			pos.x += forward.x;
			pos.z += forward.y;
		}

		if (jshInput::IsKey(JSH_KEY_SPACE)) {
			pos.y += vSpeed;
		}
		if (jshInput::IsKey(JSH_KEY_SHIFT)) {
			pos.y -= vSpeed;
		}

		trans.SetPosition(pos);

		jsh::vec3 rot = trans.GetLocalRotation();

		jsh::vec2 dragged = jshInput::MouseDragged();

		rot.y += dragged.x * hSensibility;
		rot.x += dragged.y * vSensibility;
		trans.SetRotation(rot);
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
			auto& transform = jshScene::GetTransform(entityID);
			vec3 rotation = transform.GetLocalRotation();
			vec3 position = transform.GetLocalPosition();

			if (abs(rotation.x) >= 90.f) rotation.x = (rotation.x > 0.f ? 1.f : -1.f) * 89.9f;

			direction = XMVector3Transform(direction, XMMatrixRotationRollPitchYaw(ToRadians(rotation.x), ToRadians(rotation.y), ToRadians(0.f)));

			const auto target = XMVECTOR(position) + direction;
			m_ViewMatrix = XMMatrixTranspose(XMMatrixLookAtLH(position, target, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
			transform.SetRotation(rotation);
		}
	}

}