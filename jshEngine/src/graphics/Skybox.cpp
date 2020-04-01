#include "Skybox.h"

#include "Graphics.h"
#include "..//components/CameraComponent.h"

namespace jsh {

	Buffer Skybox::s_VertexBuffer;
	Buffer Skybox::s_IndexBuffer;
	Buffer Skybox::s_ConstantBuffer;
	InputLayout Skybox::s_InputLayout;

	struct {
		XMMATRIX vm;
		XMMATRIX pm;
	} g_ConstantData;

	void Skybox::Initialize()
	{
		float vData[] = {
			-0.5f,  0.5f,  0.5f, 0.25f, 0.3333333333f,
			 0.5f,  0.5f,  0.5f, 0.5f, 0.3333333333f,
			-0.5f, -0.5f,  0.5f, 0.25f, 0.6666666666f,
			 0.5f, -0.5f,  0.5f, 0.5f, 0.6666666666f,
								 
			 0.5f,  0.5f, -0.5f, 0.75f, 0.3333333333f,
			-0.5f,  0.5f, -0.5f, 1.f, 0.3333333333f,
			 0.5f, -0.5f, -0.5f, 0.75f, 0.6666666666f,
			-0.5f, -0.5f, -0.5f, 1.f, 0.6666666666f,

			-0.5f,  0.5f, -0.5f, 0.f, 0.3333333333f,
			-0.5f,  0.5f,  0.5f, 0.25f, 0.3333333333f,
			-0.5f, -0.5f, -0.5f, 0.f, 0.6666666666f,
			-0.5f, -0.5f,  0.5f, 0.25f, 0.6666666666f,

			 0.5f,  0.5f,  0.5f, 0.5f, 0.3333333333f,
			 0.5f,  0.5f, -0.5f, 0.75f, 0.3333333333f,
			 0.5f, -0.5f,  0.5f, 0.5f, 0.6666666666f,
			 0.5f, -0.5f, -0.5f, 0.75f, 0.6666666666f,

			-0.5f, -0.5f,  0.5f, 0.25f, 0.6666666666f,
			 0.5f, -0.5f,  0.5f, 0.5f, 0.6666666666f,
			-0.5f, -0.5f, -0.5f, 0.25f, 1.f,
			 0.5f, -0.5f, -0.5f, 0.5f, 1.f,

			-0.5f,  0.5f, -0.5f, 0.25f, 0.f,
			 0.5f,  0.5f, -0.5f, 0.5f, 0.f,
			-0.5f,  0.5f,  0.5f, 0.25f, 0.3333333333f,
			 0.5f,  0.5f,  0.5f, 0.5f, 0.3333333333f,

		};

		uint32 iData[] = {
			0, 1, 2, 
			2, 1, 3, 

			4, 5, 6,
			6, 5, 7,

			8, 9, 10,
			10, 9, 11,

			12, 13, 14,
			14, 13, 15,

			16, 17, 18,
			18, 17, 19,

			20, 21, 22,
			22, 21, 23,

		};

		// scale skybox
		constexpr float SCALE = 1000.f;
		for (uint32 i = 0; i < 24 * 5; i += 5) {
			vData[i] *= SCALE;
			vData[i+1] *= SCALE;
			vData[i+2] *= SCALE;
		}

		s_VertexBuffer = jshGraphics::CreateBuffer(vData, 24 * 5 * sizeof(float), 5 * sizeof(float), JSH_USAGE_IMMUTABLE, JSH_BUFFER_TYPE_VERTEX);
		s_IndexBuffer = jshGraphics::CreateBuffer(iData, std::size(iData) * sizeof(uint32), sizeof(uint32), JSH_USAGE_IMMUTABLE, JSH_BUFFER_TYPE_INDEX);

		const Shader& shader = jshGraphics::GetShader("SkyboxShader");
		const JSH_INPUT_ELEMENT_DESC desc[] = {
			{"Position", 0, JSH_FORMAT_R32G32B32_FLOAT, 0u, true, 0u, 0u},
			{"TexCoord", 0, JSH_FORMAT_R32G32_FLOAT, 0u, true, 3 * sizeof(float), 0u}
		};
		s_InputLayout = jshGraphics::CreateInputLayout(desc, 2u, shader.vs);

		s_ConstantBuffer = jshGraphics::CreateBuffer(&g_ConstantData, sizeof(g_ConstantData), 0u, JSH_USAGE_DEFAULT, JSH_BUFFER_TYPE_CONSTANT);
	}

	Skybox::Skybox() : m_Texture(jsh::INVALID_TEXTURE) {}

	void Skybox::Render(CameraComponent* camera)
	{
		jshGraphics::BindTexture(m_Texture, 0u, JSH_SHADER_TYPE_PIXEL);
		jshGraphics::BindVertexBuffer(s_VertexBuffer, 0u);
		jshGraphics::BindIndexBuffer(s_IndexBuffer);
		const Shader& shader = jshGraphics::GetShader("SkyboxShader");
		jshGraphics::BindVertexShader(shader.vs);
		jshGraphics::BindPixelShader(shader.ps);
		jshGraphics::BindInputLayout(s_InputLayout);

		g_ConstantData.pm = camera->GetProjectionMatrix();
		g_ConstantData.vm = camera->GetViewMatrix();
		jshGraphics::UpdateConstantBuffer(s_ConstantBuffer, &g_ConstantData);
		jshGraphics::BindConstantBuffer(s_ConstantBuffer, 0u, JSH_SHADER_TYPE_VERTEX);

		jshGraphics::DrawIndexed(36u);
	}

	void Skybox::SetTexture(Texture texture) noexcept
	{
		m_Texture = texture;
	}

}