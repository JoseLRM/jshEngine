#pragma once

#include "Scene.h"

namespace jsh {

	class CameraComponent : public Component<CameraComponent> {

		XMMATRIX m_ProjectionMatrix;
		XMMATRIX m_ViewMatrix;

		bool m_3D = true;
		bool m_Orthographic = false;

		float m_Dimension = 1.f;
		float m_Aspect = 1080.f / 720.f;
		float m_Near = 0.01f;
		float m_Far = 100000.f;

	public:
		void UpdateFirstPerson3D(float hSensibility, float vSensibility, float hSpeed, float vSpeed, float dt) noexcept;

		inline void SetPerspectiveMatrix() noexcept { m_Orthographic = false; }
		inline void SetOrthographicMatrix() noexcept { m_Orthographic = true; }
		inline void Set2D() noexcept { m_3D = false; }
		inline void Set3D() noexcept { m_3D = true; }

		inline bool IsOrthographic() const noexcept { return m_Orthographic; }
		inline bool IsPerspective() const noexcept { return !m_Orthographic; }
		inline bool Is2D() const noexcept { return !m_3D; }
		inline bool Is3D() const noexcept { return m_3D; }

		inline void SetAspect(float aspect) noexcept { m_Aspect = aspect; }
		void SetDimension(float dimension) noexcept;
		inline void SetDimension(float width, float height) noexcept
		{
			SetAspect(width / height);
			SetDimension(width);
		}
		void SetNear(float near) noexcept;
		void SetFar(float far) noexcept;
		void SetFieldOfView(float fov) noexcept;

		inline float GetAspect() const noexcept { return m_Aspect; }
		inline float GetDimension() const noexcept { return m_Dimension; }
		inline float GetNear() const noexcept { return m_Near; }
		inline float GetFar() const noexcept { return m_Far; }
		inline float GetFieldOfView() const noexcept { return (atan((m_Dimension / 2.f) / m_Near)) * 2.f; }

		vec2 GetMousePos() const noexcept;

		void UpdateMatrices() noexcept;

		inline const XMMATRIX& GetProjectionMatrix() const noexcept { return m_ProjectionMatrix; }
		inline const XMMATRIX& GetViewMatrix() const noexcept { return m_ViewMatrix; }

	private:
		void UpdateMatrices2D() noexcept;
		void UpdateMatrices3D() noexcept;

#ifdef  JSH_IMGUI
	public:
		void ShowInfo() override;
#endif 

	};
	jshDefineComponent(CameraComponent);

}