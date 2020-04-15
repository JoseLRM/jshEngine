#include "RenderGraph.h"

#include "TaskSystem.h"

namespace jsh {

	RenderGraph::RenderGraph() {}
	RenderGraph::~RenderGraph() 
	{
		for (uint32 i = 0; i < m_RenderPasses.size(); ++i) {
			delete m_RenderPasses[i];
		}
	}

	void RenderGraph::Render()
	{
		TaskList* taskList = jshTask::CreateTaskList();
		for (uint32 i = 0; i < m_RenderPasses.size(); ++i) {
			m_RenderPasses[i]->cmd = jshGraphics::BeginCommandList();
			//taskList->Add([i, this]() { m_RenderPasses[i]->Run(); });
			m_RenderPasses[i]->Run();
		}
		jshTask::Execute(taskList);
		jshTask::Wait();
	}

}