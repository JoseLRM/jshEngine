#pragma once

#include "GraphicsDesc.h"

#include <vector>

namespace jsh {

	struct OutputMode {
		JSH_FORMAT format;
		uvec2 resolution;
		uint32 refreshRateNumerator;
		uint32 refreshRateDenominator;
		JSH_MODE_SCALING scaling;
		JSH_MODE_SCANLINE_ORDER scanlineOrdering;

		void* ptr = nullptr;

		bool operator==(const OutputMode& mode) const noexcept {
			return format == mode.format
				&& resolution.x == mode.resolution.x
				&& resolution.y == mode.resolution.y
				&& refreshRateNumerator == mode.refreshRateNumerator
				&& refreshRateDenominator == mode.refreshRateDenominator
				&& scaling == mode.scaling
				&& scanlineOrdering == mode.scanlineOrdering;
		}
	};

	class GraphicsAdapter {
		std::vector<OutputMode> m_Modes;

	public:
		void AddMode(const OutputMode& mode);
		const std::vector<OutputMode>& GetModes() const;

		void* ptr = nullptr;

	};

}


namespace jshGraphics {

	namespace _internal {
		void AddAdapter(const jsh::GraphicsAdapter& adapter);
	}
	const std::vector<jsh::GraphicsAdapter>& GetAdapters();

	void SetOutputMode(const jsh::OutputMode& mode);
	const jsh::OutputMode& GetOutputMode();
	
	jsh::uvec2 GetMonitorResolution();

}