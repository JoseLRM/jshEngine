#pragma once

#include "GraphicsObjects.h"

namespace jsh {

	class CameraComponent;

	class Skybox {
		
		static Buffer s_VertexBuffer;
		static Buffer s_IndexBuffer;
		static Buffer s_ConstantBuffer;
		static InputLayout s_InputLayout;
		Texture m_Texture;

#ifdef JSH_ENGINE
	public:
#endif 
		static void Initialize();

	public:
		Skybox();

		void Render(CameraComponent* camera);
		void SetTexture(Texture texture) noexcept;

	};

}