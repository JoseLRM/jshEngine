#pragma once

#include "GraphicsDesc.h"

namespace jsh {

	struct Resource {
		void* ptr = nullptr;
		JSH_SUBRESOURCE_DATA data = {};

		void Clear();

	};

	struct Buffer : public Resource {
		JSH_BUFFER_DESC desc = {};
	};

}

namespace jshGraphics {

#ifdef JSH_ENGINE
	bool Initialize();
	bool Close();

	void Prepare();
#endif

	void SetClearScreenColor(float c = 0.f);
	void SetClearScreenColor(float r, float g, float b);

	void SetTopology(JSH_TOPOLOGY topology);

	void CreateBuffer(jsh::Buffer& buffer);
	void CreateVertexBuffer(jsh::Buffer& buffer, void* pData, uint32 bufferSize, uint32 stride);
	void CreateIndexBuffer(jsh::Buffer& buffer, void* pData, uint32 indexSize, uint32 indexCount);

	void* GetDevice();
	void* GetContext();

}