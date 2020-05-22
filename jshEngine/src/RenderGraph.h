#pragma once

#include "Graphics.h"

namespace jsh {

	struct CameraComponent;
	class RenderGraph;
	class Technique;

	typedef void(*RenderPass)(jsh::CommandList cmd);

	struct LightBufferData {
		jsh::vec3 lightPos;
		float smoothness;
		float range;
		float intensity;
		uint32 type;
		float spotRange;
		jsh::vec4 color;
		jsh::vec4 direction;
	};

	class RenderGraph {
		struct TechniqueNode {
			Technique* technique = nullptr;
			
			std::vector<TechniqueNode*> depencences;
			std::vector<TechniqueNode*> childs;

			TechniqueNode() = default;
			TechniqueNode(Technique*);
		};
		std::vector<TechniqueNode> m_RootNodes;

		std::vector<Technique*> m_Techniques;

		// Globals
		struct {
			XMMATRIX vm;
			vec4 position;
		} m_CameraBufferData;

		jsh::Buffer						m_CameraBuffer;
		std::vector<LightBufferData>	m_Lights;

		jsh::Camera*		m_CurrentCamera = nullptr;

	public:
		~RenderGraph();

		void Create();
		void Render(Camera* camera);

		inline Buffer& GetCameraBuffer() noexcept { return m_CameraBuffer; }
		inline std::vector<LightBufferData>& GetLights() noexcept { return m_Lights; }

		inline Camera* GetCamera() const noexcept { return m_CurrentCamera; }

		void AddTechnique(Technique* technique) noexcept;

	};

}