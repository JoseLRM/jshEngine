#include "Renderer.h"

#include "..//ecs/Scene.h"
#include "..//components/Components.h"
#include "Graphics.h"
#include "..//utils/AssetLoader.h"
#include "..//Input.h"

using namespace jsh;

namespace jshRenderer {

	Buffer cvBuffer;
	alignas(16) struct {
		XMMATRIX tm;
		XMMATRIX vm;
		XMMATRIX pm;
	} cvData;

	Buffer cpBuffer;
	alignas(16) struct {
		vec4 lightPos;
		struct {
			float quadratic;
			float linear;
			float constant;
		}att;
		float intensity;
		vec4 color;
	} cpData;

	CameraComponent* g_MainCamera = nullptr;
	FrameBuffer g_MainFB;
	SamplerState sampler;
	Viewport viewport;

	bool Initialize()
	{
		cvData.pm = XMMatrixTranspose(XMMatrixPerspectiveFovLH(70, 1080.f / 720.f, 5.f, 2000.f));
		cvData.tm = XMMatrixTranspose(XMMatrixTranslation(0.f, 0.f, 20.f));
		cvData.vm = XMMatrixTranspose(XMMatrixIdentity());

		JSH_BUFFER_DESC cvDesc;
		cvDesc.BindFlags = JSH_BIND_CONSTANT_BUFFER;
		cvDesc.ByteWidth = sizeof(cvData);
		cvDesc.CPUAccessFlags = 0u;
		cvDesc.MiscFlags = 0u;
		cvDesc.StructureByteStride = 0u;
		cvDesc.Usage = JSH_USAGE_DEFAULT;
		JSH_SUBRESOURCE_DATA cvSData;
		cvSData.pSysMem = &cvData;
		jshGraphics::CreateBuffer(&cvDesc, &cvSData, &cvBuffer);

		cpData.color.x = 1.f;
		cpData.color.y = 1.f;
		cpData.color.z = 1.f;
		cpData.intensity = 10.f;
		cpData.lightPos.x = 0.f;
		cpData.lightPos.y = 0.f;
		cpData.lightPos.z = 0.f;
		cpData.att.constant = 1.f;
		cpData.att.linear = 0.3f;
		cpData.att.quadratic = 0.016f;
		JSH_BUFFER_DESC cpDesc;
		cpDesc.BindFlags = JSH_BIND_CONSTANT_BUFFER;
		cpDesc.ByteWidth = sizeof(cpData);
		cpDesc.CPUAccessFlags = 0u;
		cpDesc.MiscFlags = 0u;
		cpDesc.StructureByteStride = 0u;
		cpDesc.Usage = JSH_USAGE_DEFAULT;
		JSH_SUBRESOURCE_DATA cpSData;
		cpSData.pSysMem = &cpData;
		jshGraphics::CreateBuffer(&cpDesc, &cpSData, &cpBuffer);
		
		g_MainFB.Create(true);
		JSH_SAMPLER_DESC samplerDesc;
		jshZeroMemory(&samplerDesc, sizeof(JSH_SAMPLER_DESC));
		samplerDesc.AddressU = JSH_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = JSH_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = JSH_TEXTURE_ADDRESS_WRAP;
		samplerDesc.Filter = JSH_FILTER_MIN_MAG_MIP_LINEAR;
		jshGraphics::CreateSamplerState(&samplerDesc, &sampler);

		jshGraphics::CreateViewport(0, 0, 1080, 720, &viewport);

		return true;
	}

	void BeginFrame()
	{
		jshImGui(jshGraphics::BeginImGui());
		jshGraphics::Begin();
		jshGraphics::BeginCommandList();
		g_MainFB.Bind(0);
		g_MainFB.Clear(0u);

		jshGraphics::BindViewport(viewport, 0, 0u);
		g_MainFB.EnableDepthTest(0);

		jshGraphics::BindSamplerState(sampler, 0u, JSH_SHADER_TYPE_PIXEL, 0u);
		jshGraphics::BindViewport(viewport, 0, 0);
	}

	void EndFrame()
	{
		jshGraphics::End();
		jshImGui(jshGraphics::EndImGui(g_MainFB.GetRTV()));
		jshGraphics::Present();	
	}

	bool Close()
	{
		return true;
	}

	void DrawScene(Scene* pScene)
	{
		MeshComponent* comp = pScene->GetComponent<MeshComponent>(1);
		Mesh mesh = comp->mesh;	

		jshGraphics::SetTopology(JSH_TOPOLOGY_TRIANGLES, 0u);
		jshGraphics::BindConstantBuffer(cvBuffer, 0, JSH_SHADER_TYPE_VERTEX, 0u);
		jshGraphics::BindConstantBuffer(cpBuffer, 0, JSH_SHADER_TYPE_PIXEL, 0u);

		mesh.rawData->Bind(0u);
		jshGraphics::BindTexture(mesh.diffuseMap, 0, JSH_SHADER_TYPE_PIXEL, 0u);
		jshGraphics::DrawIndexed(mesh.rawData->GetIndexCount(), 0u);
	}

	void SetCamera(jsh::CameraComponent* camera)
	{
		camera->UpdateMatrices();
		g_MainCamera = camera;
	}

}