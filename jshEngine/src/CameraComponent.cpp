#include "common.h"

#include "CameraComponent.h"

namespace jsh {
	CameraComponent::CameraComponent()
	{
		
	}
	void CameraComponent::UpdateMatrices()
	{
		Transform& trans = jshScene::GetTransform(entity);
		m_Camera.position = trans.GetWorldPosition();
		m_Camera.scale2D = vec2(trans.GetWorldScale());
		//TODO: World rotation
		m_Camera.rotation = trans.GetLocalRotation();

		m_Camera.UpdateMatrices();
	}

	void CameraComponent::UpdateFirstPerson3D(float hSensibility, float vSensibility, float hSpeed, float vSpeed, float dt) noexcept
	{
		vSpeed *= dt;
		hSpeed *= dt;

		uint8 front = 0u;
		uint8 right = 0u;
		jsh::Transform& trans = jshScene::GetTransform(entity);
		float direction = ToDegrees(trans.GetLocalRotation().y);
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

		rot.y += ToRadians(dragged.x * hSensibility);
		rot.x += ToRadians(dragged.y * vSensibility);
		trans.SetRotation(rot);
	}

	void CameraComponent::UpdateFreeCamera2D(float hSpeed, float vSpeed, float zoomSpeed, float dt) noexcept
	{
		zoomSpeed *= dt;

		jsh::vec3 vel;

		if (jshInput::IsKey('W')) {
			vel.y += vSpeed;
		}
		if (jshInput::IsKey('S')) {
			vel.y -= vSpeed;
		}
		if (jshInput::IsKey('D')) {
			vel.x += vSpeed;
		}
		if (jshInput::IsKey('A')) {
			vel.x -= vSpeed;
		}

		vel *= dt;
		jsh::Transform& trans = jshScene::GetTransform(entity);
		trans.SetPosition(trans.GetLocalPosition() + vel);
	}

#ifdef  JSH_IMGUI
		void CameraComponent::ShowInfo()
		{
			m_Camera.ShowImGuiWindow();
			if (ImGui::Button("Set Main Camera")) jshRenderer::SetCamera(this);
		}

#endif 

}