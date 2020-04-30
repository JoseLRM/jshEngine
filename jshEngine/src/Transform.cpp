#include "Scene.h"

namespace jsh {

	void Transform::operator=(const Transform& other)
	{
		m_LocalPosition = other.m_LocalPosition;
		m_LocalRotation = other.m_LocalRotation;
		m_LocalScale = other.m_LocalScale;
		m_Modified = true;
	}
	void Transform::operator=(Transform&& other)
	{
		m_LocalPosition = other.m_LocalPosition;
		m_LocalRotation = other.m_LocalRotation;
		m_LocalScale = other.m_LocalScale;
		entity = other.entity;
		m_Modified = true;
	}

	XMMATRIX Transform::GetLocalMatrix() const noexcept
	{
		XMVECTOR radRotation = XMVectorSet(m_LocalRotation.x, m_LocalRotation.y, m_LocalRotation.z, 1.f);
		return XMMatrixScalingFromVector(GetLocalScaleDXV()) * XMMatrixRotationRollPitchYawFromVector(radRotation)
			* XMMatrixTranslationFromVector(GetLocalPositionDXV());
	}

	vec3 Transform::GetWorldPosition() noexcept
	{
		if (m_Modified) UpdateWorldMatrix();
		return *(vec3*)& m_WorldMatrix._41;
	}
	vec3 Transform::GetWorldRotation() noexcept
	{
		if (m_Modified) UpdateWorldMatrix();
		return *(vec3*)& GetWorldRotationDXV();
	}
	vec3 Transform::GetWorldScale() noexcept
	{
		if (m_Modified) UpdateWorldMatrix();
		return *(vec3*)& GetWorldScaleDXV();
	}
	XMVECTOR Transform::GetWorldPositionDXV() noexcept
	{
		if (m_Modified) UpdateWorldMatrix();
		XMVECTOR scale;
		XMVECTOR rotation;
		XMVECTOR position;

		XMMatrixDecompose(&scale, &rotation, &position, XMLoadFloat4x4(&m_WorldMatrix));

		return position;
	}
	XMVECTOR Transform::GetWorldRotationDXV() noexcept
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
	XMVECTOR Transform::GetWorldScaleDXV() noexcept
	{
		if (m_Modified) UpdateWorldMatrix();
		XMVECTOR scale;
		XMVECTOR rotation;
		XMVECTOR position;

		XMMatrixDecompose(&scale, &rotation, &position, XMLoadFloat4x4(&m_WorldMatrix));

		return scale;
	}

	XMMATRIX Transform::GetWorldMatrix() noexcept
	{
		if (m_Modified) UpdateWorldMatrix();
		return XMLoadFloat4x4(&m_WorldMatrix);
	}

	void Transform::SetPosition(const vec3& position) noexcept
	{
		m_Modified = true;
		m_LocalPosition = *(XMFLOAT3*)& position;
	}
	void Transform::SetRotation(const vec3& rotation) noexcept
	{
		m_Modified = true;
		m_LocalRotation = *(XMFLOAT3*)& rotation;
	}
	void Transform::SetScale(const vec3& scale) noexcept
	{
		m_Modified = true;
		m_LocalScale = *(XMFLOAT3*)& scale;
	}

	void Transform::UpdateWorldMatrix()
	{
		m_Modified = false;

		XMMATRIX m = GetLocalMatrix();

		auto& list = jshScene::_internal::GetEntityDataList();
		EntityData& entityData = list[entity];
		Entity parent = entityData.parent;

		if (parent != INVALID_ENTITY) {
			XMMATRIX mp = list[parent].transform.GetWorldMatrix();
			m = m * mp;
		}
		XMStoreFloat4x4(&m_WorldMatrix, m);

		// Set all the sons modified
		if (entityData.sonsCount == 0) return;

		auto& entities = jshScene::_internal::GetEntitiesList();
		for (uint32 i = 0; i < entityData.sonsCount; ++i) {
			EntityData& ed = list[entities[entityData.handleIndex + 1 + i]];
			ed.transform.m_Modified = true;
		}

	}

#ifdef JSH_IMGUI
	void Transform::ShowInfo()
	{
		ImGui::DragFloat3("Position", &m_LocalPosition.x, 0.75f);

		vec3 rot = *(vec3*)(&m_LocalRotation);
		rot.x = ToDegrees(rot.x);
		rot.y = ToDegrees(rot.y);
		rot.z = ToDegrees(rot.z);
		ImGui::DragFloat3("Rotation", &rot.x, 0.25f);
		rot.x = ToRadians(rot.x);
		rot.y = ToRadians(rot.y);
		rot.z = ToRadians(rot.z);
		m_LocalRotation = *(XMFLOAT3*)(&rot);

		ImGui::DragFloat3("Scale", &m_LocalScale.x, 0.25f);

		if (ImGui::Button("Reset")) {
			m_LocalRotation = { 0.f, 0.f, 0.f };
			m_LocalScale = { 1.f, 1.f, 1.f };
		}

		UpdateWorldMatrix();
	}
#endif

}