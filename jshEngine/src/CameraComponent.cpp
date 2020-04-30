#include "CameraComponent.h"

#include "Window.h"
#include "Input.h"
#include "Renderer.h"
#include "Engine.h"

namespace jsh {

	void CameraComponent::SetDimension(float dimension) noexcept
	{
		m_Dimension = dimension;
	}
	void CameraComponent::SetFieldOfView(float fov) noexcept
	{
		m_Near = tan(ToRadians(fov / 2.f)) * m_Dimension / 2.f;
	}

	void CameraComponent::SetNear(float near) noexcept 
	{ 
		m_Near = near; 
	}
	void CameraComponent::SetFar(float far) noexcept { m_Far = far; }

	vec2 CameraComponent::GetMousePos() const noexcept
	{
		jsh::vec3 pos = jshScene::GetTransform(entity).GetWorldPosition();
		return jshInput::MousePos() * vec2(m_Dimension, m_Dimension / m_Aspect) + vec2(pos.x, pos.y);
	}

	void CameraComponent::UpdateFirstPerson3D(float hSensibility, float vSensibility, float hSpeed, float vSpeed, float dt) noexcept
	{

		vSpeed *= dt;
		hSpeed *= dt;

		uint8 front = 0u;
		uint8 right = 0u;
		jsh::Transform& trans = jshScene::GetTransform(entity);
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

	void CameraComponent::UpdateMatrices() noexcept
	{
		if (Is3D()) {
			UpdateMatrices3D();
		}
		else {
			UpdateMatrices2D();
		}
	}

	void CameraComponent::UpdateMatrices2D() noexcept
	{
		if (IsOrthographic()) {
			Transform& trans = jshScene::GetTransform(entity);
			vec3 pos = trans.GetLocalPosition();

			m_ProjectionMatrix = XMMatrixTranspose(XMMatrixOrthographicLH(m_Dimension, m_Dimension / m_Aspect, m_Near, m_Far));
			m_ViewMatrix = XMMatrixTranspose(XMMatrixTranslation(-pos.x, -pos.y, 1.f));
		}
	}
	void CameraComponent::UpdateMatrices3D() noexcept
	{
		if (IsOrthographic()) {
			m_ProjectionMatrix = XMMatrixTranspose(XMMatrixOrthographicLH(m_Dimension, m_Dimension / m_Aspect, m_Near, m_Far));
		}
		else {
			if (m_Near <= 0.f) m_Near = 0.001f;
			if (m_Far <= 0.f) m_Far = 0.001f;

			m_ProjectionMatrix = XMMatrixTranspose(XMMatrixPerspectiveLH(m_Dimension, m_Dimension / m_Aspect, m_Near, m_Far));
		}

		XMVECTOR direction = XMVectorSet(0.f, 0.f, 1.f, 0.f);
		auto& transform = jshScene::GetTransform(entity);
		vec3 rotation = transform.GetLocalRotation();
		vec3 position = transform.GetLocalPosition();

		if (abs(rotation.x) >= 90.f) rotation.x = (rotation.x > 0.f ? 1.f : -1.f) * 89.9f;

		direction = XMVector3Transform(direction, XMMatrixRotationRollPitchYaw(ToRadians(rotation.x), ToRadians(rotation.y), ToRadians(0.f)));

		const auto target = XMVECTOR(position) + direction;
		m_ViewMatrix = XMMatrixTranspose(XMMatrixLookAtLH(position, target, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
		transform.SetRotation(rotation);
	}

#ifdef  JSH_IMGUI
		void CameraComponent::ShowInfo()
		{
			ImGui::DragFloat("Dimension", &m_Dimension, 0.2f);
			ImGui::DragFloat("Aspect", &m_Aspect, 0.001f, 0.01f, 0.99f);
			float fov = GetFieldOfView();
			if (ImGui::DragFloat("Fov", &fov, 0.25f, 0.01f, FLT_MAX)) {
				SetFieldOfView(fov);
			}
			ImGui::DragFloat("Near", &m_Near, 0.05f, 0.01f, FLT_MAX);
			ImGui::DragFloat("Far", &m_Far, 1.f, 0.01f, FLT_MAX);

			if (ImGui::Button(m_Orthographic ? "Change to Perspective" : "Change to Orthographic")) {
				m_Orthographic = !m_Orthographic;
			}
			if (ImGui::Button(m_3D ? "Set 2D" : "Set 3D")) {
				m_3D = !m_3D;
			}
			if (ImGui::Button("Set Main Camera")) jshEngine::GetRenderer()->SetMainCamera(entity);
		}

#endif 

}