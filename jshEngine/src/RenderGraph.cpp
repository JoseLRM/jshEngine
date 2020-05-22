#include "common.h"

#include "RenderGraph.h"
#include "Technique.h"

namespace jsh {

	RenderGraph::TechniqueNode::TechniqueNode(Technique* t) : technique(t) {}

	RenderGraph::~RenderGraph()
	{
		//TODO: Clear nodes

		for (uint32 i = 0; i < m_Techniques.size(); ++i) {
			m_Techniques[i]->Close();
			delete m_Techniques[i];
		}
	}

	void RenderGraph::Create()
	{
		// GLOBAL BUFFERS
		{
			JSH_BUFFER_DESC cameraDesc;
			cameraDesc.BindFlags = JSH_BIND_CONSTANT_BUFFER;
			cameraDesc.ByteWidth = sizeof(m_CameraBufferData);
			cameraDesc.CPUAccessFlags = 0u;
			cameraDesc.MiscFlags = 0u;
			cameraDesc.StructureByteStride = 0u;
			cameraDesc.Usage = JSH_USAGE_DEFAULT;
			JSH_SUBRESOURCE_DATA cameraSData;
			cameraSData.pSysMem = &m_CameraBufferData;
			jshGraphics::CreateBuffer(&cameraDesc, &cameraSData, &m_CameraBuffer);
		}
	}

	void RenderGraph::Render(Camera* camera)
	{
		//GLOBAL BUFFERS
		bool doFrame = true;
		m_CurrentCamera = camera;

		if (camera) {

			// command list to update globals
			CommandList cmd = 0;

			// camera
			m_CameraBufferData.vm = XMMatrixTranspose(camera->GetProjectionMatrix());
			m_CameraBufferData.position = { camera->position.x, camera->position.y, camera->position.z, 1.f };
			jshGraphics::UpdateBuffer(m_CameraBuffer, &m_CameraBufferData, 0u, cmd);

			// lights
			m_Lights.clear();
			auto& lightsList = jshScene::_internal::GetComponentsList()[LightComponent::ID];

			const uint32 componentSize = uint32(LightComponent::SIZE);

			XMMATRIX viewMatrix = camera->GetViewMatrix();

			for (uint32 i = 0; i < lightsList.size(); i += componentSize) {
				LightComponent* lightComp = reinterpret_cast<LightComponent*>(&lightsList[i]);

				if (lightComp->lightType == 0) continue;

				Transform& trans = jshScene::GetTransform(lightComp->entity);

				XMMATRIX tm = trans.GetWorldMatrix() * viewMatrix;

				XMVECTOR position;
				XMVECTOR rotationV;
				XMVECTOR scale;
				XMMatrixDecompose(&scale, &rotationV, &position, tm);

				vec3 rotation;
				if (lightComp->lightType != JSH_LIGHT_TYPE_POINT) {
					//TODO: Get World Rotation
					rotation = trans.GetLocalRotation();
					XMVECTOR rotated = XMVector3Rotate(XMVectorSet(0.f, 0.f, -1.f, 1.f), XMQuaternionRotationAxis(rotationV, 0.f));
					rotation = rotated;
				}

				LightBufferData light;
				light.color = vec4(lightComp->color);
				light.direction = vec4(rotation.x, rotation.y, rotation.z, 0.f);
				light.lightPos = position;
				light.smoothness = lightComp->smoothness;
				light.range = lightComp->range;
				light.intensity = lightComp->intensity;
				light.spotRange = lightComp->spotRange;
				light.type = lightComp->lightType;
				m_Lights.emplace_back(light);
			}
		}
		else {
			jshDebug::LogE("Invalid parameters in RenderGraph, you're missing something...");
			doFrame = false;
		}

		if (!doFrame) return;

		// UPDATE TECHNIQUES
		{
			for (uint32 i = 0; i < m_Techniques.size(); ++i) {
				m_Techniques[i]->Load();
			}
		}

		// RENDER TECHNIQUES
		{
			std::vector<TechniqueNode*> nextNodes;
			std::vector<TechniqueNode*> renderingNodes;

			// add root nodes
			for (uint32 i = 0; i < m_RootNodes.size(); ++i) {
				m_RootNodes[i].technique->Run();

				renderingNodes.push_back(&m_RootNodes[i]);

				for (uint32 j = 0; j < m_RootNodes[i].childs.size(); ++j) {
					TechniqueNode* child = m_RootNodes[i].childs[j];

					auto it = std::find(nextNodes.begin(), nextNodes.end(), child);

					if(it == nextNodes.end())
						nextNodes.push_back(child);
				}
			}

			while (!nextNodes.empty()) {
				for (uint32 i = 0; i < nextNodes.size(); ++i) {
					TechniqueNode* node = nextNodes[i];

					bool render = true;

					for (uint32 j = 0; j < node->depencences.size(); ++j) {
						if (node->depencences[j]->technique->IsRendering()) {
							render = false;
							break;
						}
					}

					if (render) {
						node->technique->Run();
						renderingNodes.push_back(node);

						nextNodes.erase(nextNodes.begin() + i);

						for (uint32 j = 0; j < node->childs.size(); ++j) {
							TechniqueNode* child = node->childs[j];
							if(std::find(nextNodes.begin(), nextNodes.end(), child) == nextNodes.end())
								nextNodes.push_back(child);
						}
					}

					for (uint32 i = 0; i < renderingNodes.size(); ++i) {
						if (!renderingNodes[i]->technique->IsRendering()) renderingNodes.erase(renderingNodes.begin() + i);
					}
				}

				std::this_thread::yield();
			}

			uint32 count = 0;
			while (count < renderingNodes.size()) {

				while (renderingNodes[count]->technique->IsRendering()) {
					std::this_thread::yield();
				}

				count++;
			}
		}
	}

	void RenderGraph::AddTechnique(Technique* technique) noexcept
	{
		technique->m_pRenderGraph = this;
		technique->Initialize();
		m_Techniques.push_back(technique);

		auto attachments = technique->m_Attachments;
		uint32 numOfAttachments = 0u;
		for (uint32 i = 0; i < JSH_GFX_ATTACHMENTS_COUNT; ++i) {
			if (attachments[i] == nullptr) break;
			numOfAttachments++;
		}


		if (numOfAttachments == 0) {
			m_RootNodes.emplace_back(technique);
			return;
		}

		TechniqueNode** dependences = new TechniqueNode * [numOfAttachments];
		jshZeroMemory(dependences, sizeof(TechniqueNode*) * numOfAttachments);

		for (uint32 att = 0; att < numOfAttachments; ++att) {
			GfxPrimitive* attachment = attachments[att];
			TechniqueNode* dependence = nullptr;

			for (uint32 i = 0; i < m_RootNodes.size(); ++i) {
				if (m_RootNodes[i].technique->HasAttachment(attachment)) {
					dependence = &m_RootNodes[i];
					break;
				}
			}

			if (dependence != nullptr) {
				bool end = false;

				while (!end)
				{
					end = true;
					for (uint32 j = 0; j < dependence->childs.size(); ++j) {
						if (dependence->childs[j]->technique->HasAttachment(attachment)) {
							dependence = dependence->childs[j];
							end = false;
							break;
						}
					}
				}
			}

			dependences[att] = dependence;
		}

		TechniqueNode* addNode = new TechniqueNode(technique);
		bool root = true;
		for (uint32 i = 0; i < numOfAttachments; ++i) {
			if (dependences[i] != nullptr) {
				root = false;
				addNode->depencences.push_back(dependences[i]);
				dependences[i]->childs.push_back(addNode);
			}
		}

		if (root) {
			m_RootNodes.push_back(*addNode);
		}
	}

}