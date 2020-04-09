#include "TransformComponent.h"

#include "..//Engine.h"
#include "..//State.h"

namespace jsh {

	TransformComponent::TransformComponent() {}
	TransformComponent::TransformComponent(float x, float y, float z) : m_LocalPosition(x, y, z) {}

	vec3 TransformComponent::GetLocalRotationDXVE() const noexcept
	{
		XMVECTOR rot;
		float r;
		XMQuaternionToAxisAngle(&rot, &r, GetLocalRotationDXV());
		return rot;
	}
	vec3 TransformComponent::GetLocalRotationE() const noexcept
	{
		return *(vec3*)&GetLocalRotationDXVE();
	}

	XMMATRIX TransformComponent::GetLocalMatrix() const noexcept
	{
		return XMMatrixScalingFromVector(GetLocalScaleDXV()) * XMMatrixRotationQuaternion(GetLocalRotationDXV())
			* XMMatrixTranslationFromVector(GetLocalPositionDXV());
	}

	vec3 TransformComponent::GetWorldPosition() noexcept 
	{ 
		if (m_Modified) UpdateWorldMatrix();
		return *(vec3*)&m_WorldMatrix._41;
	}
	vec4 TransformComponent::GetWorldRotation() noexcept 
	{ 
		if (m_Modified) UpdateWorldMatrix();
		return *(vec4*)&GetWorldRotationDXV(); 
	}
	vec3 TransformComponent::GetWorldScale() noexcept 
	{ 
		if (m_Modified) UpdateWorldMatrix();
		return *(vec3*)& GetWorldScaleDXV();
	}
	XMVECTOR TransformComponent::GetWorldPositionDXV() noexcept
	{
		XMVECTOR scale;
		XMVECTOR rotation;
		XMVECTOR position;

		XMMatrixDecompose(&scale, &rotation, &position, XMLoadFloat4x4(&m_WorldMatrix));

		return position;
	}
	XMVECTOR TransformComponent::GetWorldRotationDXV() noexcept
	{
		XMVECTOR scale;
		XMVECTOR rotation;
		XMVECTOR position;

		XMMatrixDecompose(&scale, &rotation, &position, XMLoadFloat4x4(&m_WorldMatrix));

		return rotation;
	}
	XMVECTOR TransformComponent::GetWorldScaleDXV() noexcept
	{
		XMVECTOR scale;
		XMVECTOR rotation;
		XMVECTOR position;

		XMMatrixDecompose(&scale, &rotation, &position, XMLoadFloat4x4(&m_WorldMatrix));

		return scale;
	}

	XMMATRIX TransformComponent::GetWorldMatrix() noexcept
	{
		if (m_Modified) UpdateWorldMatrix();
		return XMLoadFloat4x4(&m_WorldMatrix);
	}

	void TransformComponent::SetPosition(const vec3& position) noexcept
	{
		m_Modified = true;
		m_LocalPosition = *(XMFLOAT3*)&position;
	}
	void TransformComponent::SetRotation(const vec4& rotation) noexcept
	{
		m_Modified = true;
		m_LocalRotation = *(XMFLOAT4*)&rotation;
	}
	void TransformComponent::SetRotationE(const vec3& rotation) noexcept
	{
		m_Modified = true;
		m_LocalRotation = *(XMFLOAT4*)&XMQuaternionRotationRollPitchYawFromVector(XMVectorSet(rotation.x, rotation.y, rotation.z, 1.f));
	}
	void TransformComponent::SetScale(const vec3& scale) noexcept
	{
		m_Modified = true;
		m_LocalScale = *(XMFLOAT3*)&scale;
	}

	void TransformComponent::UpdateWorldMatrix()
	{
		m_Modified = false;
		Scene& scene = jshEngine::GetScene();
		Entity parent = scene.GetEntityParent(entityID);

		XMMATRIX m = GetLocalMatrix();

		if (parent != INVALID_ENTITY) {
			TransformComponent* parentComp = scene.GetComponent<TransformComponent>(scene.GetEntityParent(entityID));
			
			if (parentComp) {
				XMMATRIX mp = parentComp->GetWorldMatrix();
				m = m * mp;
			}
		}
		XMStoreFloat4x4(&m_WorldMatrix, m);
	}

#ifdef JSH_IMGUI
	void TransformComponent::ShowInfo()
	{
		m_Modified = true;
		ImGui::DragFloat3("Position", &m_LocalPosition.x, 0.75f);
		vec3 rotation = GetLocalRotationE();
		ImGui::DragFloat3("Rotation", &rotation.x, 0.25f);
		ImGui::DragFloat3("Scale", &m_LocalScale.x, 0.25f);

		SetRotationE(rotation);
	}
#endif

}