#include "common.h"
#include "GraphicsAdapter.h"

#include "GraphicsAPI_dx11.h"
#include "EventSystem.h"

namespace jsh {

	void GraphicsAdapter::AddMode(const OutputMode& mode) {
		bool duplicated = false;
		for (auto it = m_Modes.begin(); it != m_Modes.end(); ++it) {
			if (mode == (*it)) {
				duplicated = true;
				break;
			}
		}

		if (!duplicated) {
			m_Modes.push_back(mode);
		}
	}

	const std::vector<OutputMode>& GraphicsAdapter::GetModes() const
	{
		return m_Modes;
	}

}

using namespace jsh;

namespace jshGraphics {

	std::vector<GraphicsAdapter> g_Adapters;
	OutputMode g_Mode;

	uvec2 g_MonitorResolution;

	namespace _internal {

		void AddAdapter(const GraphicsAdapter& adapter)
		{
			g_Adapters.push_back(adapter);

			for (auto it = adapter.GetModes().begin(); it != adapter.GetModes().end(); ++it) {
				if ((*it).resolution.x > g_MonitorResolution.x) g_MonitorResolution = (*it).resolution;
			}
		}

	}
	using namespace _internal;

	const std::vector<jsh::GraphicsAdapter>& GetAdapters()
	{
		return g_Adapters;
	}

	void SetOutputMode(const jsh::OutputMode& mode)
	{
		if(g_Mode == mode) return;

		if (jshEngine::IsInitialized()) {
			jshGraphics_dx11::SetOutputMode(mode);

			jsh::OutputModeEvent e(mode);
			jshEvent::Dispatch(e);
		}

		g_Mode = mode;
	}
	const jsh::OutputMode& GetOutputMode()
	{
		return g_Mode;
	}
	uvec2 GetMonitorResolution()
	{
		return g_Mode.resolution;
	}

}