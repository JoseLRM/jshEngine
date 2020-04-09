#pragma once

#include "../ecs/Scene.h"

namespace jsh {

	struct TransformComponent : public Component<TransformComponent> {

	private:
		XMFLOAT3 m_LocalPosition = { 0.f, 0.f, 0.f };
		XMFLOAT4 m_LocalRotation = { 0.f, 0.f, 0.f, 1.f };
		XMFLOAT3 m_LocalScale = { 1.f, 1.f, 1.f };

		XMFLOAT4X4 m_WorldMatrix;

		bool m_Modified = true;

	public:
		TransformComponent();
		TransformComponent(float x, float y, float z);

		// getters
		inline const vec3& GetLocalPosition() const noexcept { return *(vec3*)&m_LocalPosition; }
		inline const vec4& GetLocalRotation() const noexcept { return *(vec4*)& m_LocalRotation; }
		inline const vec3& GetLocalScale() const noexcept { return *(vec3*)& m_LocalScale; }
		inline XMVECTOR GetLocalPositionDXV() const noexcept { return XMLoadFloat3((XMFLOAT3*)&m_LocalPosition); }
		inline XMVECTOR GetLocalRotationDXV() const noexcept { return XMLoadFloat4((XMFLOAT4*)&m_LocalRotation); }
		inline XMVECTOR GetLocalScaleDXV() const noexcept { return XMLoadFloat3((XMFLOAT3*)&m_LocalScale); }
		XMMATRIX GetLocalMatrix() const noexcept;
		vec3 GetLocalRotationE() const noexcept;
		vec3 GetLocalRotationDXVE() const noexcept;

		vec3 GetWorldPosition() noexcept;
		vec4 GetWorldRotation() noexcept;
		vec3 GetWorldScale() noexcept;
		XMVECTOR GetWorldPositionDXV() noexcept;
		XMVECTOR GetWorldRotationDXV() noexcept;
		XMVECTOR GetWorldScaleDXV() noexcept;
		XMMATRIX GetWorldMatrix() noexcept;

		// setters
		void SetPosition(const vec3& position) noexcept;
		void SetRotation(const vec4& rotation) noexcept;
		void SetRotationE(const vec3& rotation) noexcept;
		void SetScale(const vec3& scale) noexcept;

		// ImGui
#ifdef JSH_IMGUI
		void ShowInfo() override;
#endif

	private:
		void UpdateWorldMatrix();

	};
	jshDefineComponent(TransformComponent);

}