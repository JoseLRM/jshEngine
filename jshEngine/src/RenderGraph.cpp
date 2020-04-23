#include "RenderGraph.h"

#include "TaskSystem.h"

namespace jsh {

	void RenderPass::AddDependence(RenderPass* renderPass) noexcept
	{
		if (renderPass == this) return;
		for (uint32 i = 0; i < m_Depencences.size(); ++i) {
			if (m_Depencences[i] == renderPass) return;
		}
		m_Depencences.push_back(renderPass);
	}

	RenderGraph::RenderGraph() {}
	RenderGraph::~RenderGraph() 
	{
		for (uint32 i = 0; i < m_RenderPasses.size(); ++i) {
			delete m_RenderPasses[i];
		}
	}

	void RenderGraph::Render()
	{
		if (m_Modified) UpdateGraph();

		for (uint32 i = 0; i < m_RenderPasses.size(); ++i) {
			m_RenderPasses[i]->Load();
		}
		jshTask::Wait();

		for (uint32 i = 0; i < m_RenderPasses.size(); ++i) {
			m_RenderPasses[i]->Render(jshGraphics::BeginCommandList());
		}
	}

	void RenderGraph::UpdateGraph()
	{
		m_Modified = false;
		//TODO: Render graph
	}

}