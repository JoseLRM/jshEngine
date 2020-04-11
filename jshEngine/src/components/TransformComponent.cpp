#include "TransformComponent.h"

#include "..//Engine.h"
#include "..//State.h"

namespace jsh {

	TransformComponent::TransformComponent() {}
	TransformComponent::TransformComponent(float x, float y, float z) : m_LocalPosition(x, y, z) {}

	XMMATRIX TransformComponent::GetLocalMatrix() const noexcept
	{
		XMVECTOR radRotation = XMVectorSet(ToRadians(m_LocalRotation.x), ToRadians(m_LocalRotation.y), ToRadians(m_LocalRotation.z), 1.f);
		return XMMatrixScalingFromVector(GetLocalScaleDXV()) * XMMatrixRotationRollPitchYawFromVector(radRotation)
			* XMMatrixTranslationFromVector(GetLocalPositionDXV());
	}

	vec3 TransformComponent::GetWorldPosition() noexcept 
	{ 
		if (m_Modified) UpdateWorldMatrix();
		return *(vec3*)&m_WorldMatrix._41;
	}
	vec3 TransformComponent::GetWorldRotation() noexcept 
	{ 
		if (m_Modified) UpdateWorldMatrix();
		return *(vec3*)&GetWorldRotationDXV(); 
	}
	vec3 TransformComponent::GetWorldScale() noexcept 
	{ 
		if (m_Modified) UpdateWorldMatrix();
		return *(vec3*)& GetWorldScaleDXV();
	}
	XMVECTOR TransformComponent::GetWorldPositionDXV() noexcept
	{
		if (m_Modified) UpdateWorldMatrix();
		XMVECTOR scale;
		XMVECTOR rotation;
		XMVECTOR position;

		XMMatrixDecompose(&scale, &rotation, &position, XMLoadFloat4x4(&m_WorldMatrix));

		return position;
	}
	XMVECTOR TransformComponent::GetWorldRotationDXV() noexcept
	{
		if (m_Modified) UpdateWorldMatrix();
		XMVECTOR scale;
		XMVECTOR quatRotation;
		XMVECTOR rotation;
		float angle;
		XMVECTOR position;

		XMMatrixDecompose(&scale, &quatRotation, &position, XMLoadFloat4x4(&m_WorldMatrix));
		XMQuaternionToAxisAngle(&rotation, &angle, quatRotation);

		return rotation;
	}
	XMVECTOR TransformComponent::GetWorldScaleDXV() noexcept
	{
		if (m_Modified) UpdateWorldMatrix();
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
	void TransformComponent::SetRotation(const vec3& rotation) noexcept
	{
		m_Modified = true;
		m_LocalRotation = *(XMFLOAT3*)&rotation;
	}
	void TransformComponent::SetScale(const vec3& scale) noexcept
	{
		m_Modified = true;
		m_LocalScale = *(XMFLOAT3*)&scale;
	}

	void TransformComponent::UpdateWorldMatrix()
	{
		m_Modified = false;
		Entity parent = jshScene::GetEntityParent(entityID);

		XMMATRIX m = GetLocalMatrix();

		if (parent != INVALID_ENTITY) {
			TransformComponent* parentComp = jshScene::GetComponent<TransformComponent>(jshScene::GetEntityParent(entityID));
			
			if (parentComp) {
				XMMATRIX mp = parentComp->GetWorldMatrix();
				m = m * mp;
			}
		}
		XMStoreFloat4x4(&m_WorldMatrix, m);

		// Set all the sons modified
		EntityData& entityData = jshScene::_internal::GetEntityDataList()[entityID];
		if (entityData.sonsCount == 0) return;

		auto& entities = jshScene::_internal::GetEntitiesList();
		for (uint32 i = 0; i < entityData.sonsCount; ++i) {
			TransformComponent* transform = jshScene::GetComponent<TransformComponent>(entities[entityData.handleIndex + 1 + i]);
			if (transform) transform->m_Modified = true;
		}

	}

#ifdef JSH_IMGUI
	void TransformComponent::ShowInfo()
	{
		ImGui::DragFloat3("Position", &m_LocalPosition.x, 0.75f);
		ImGui::DragFloat3("Rotation", &m_LocalRotation.x, 0.25f);
		ImGui::DragFloat3("Scale", &m_LocalScale.x, 0.25f);

		if (ImGui::Button("Reset")) {
			m_LocalRotation = { 0.f, 0.f, 0.f };
			m_LocalScale = { 1.f, 1.f, 1.f };
		}

		UpdateWorldMatrix();
	}
#endif

}