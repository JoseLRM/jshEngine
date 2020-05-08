#pragma once

#include "GraphicsObjects.h"

namespace jsh {

	class RenderQueue {

		struct Instance {
			RawData* rawData;
			Material* material;
			InputLayout il;
			Transform* trans;

			Instance(RawData* r, Material* m, InputLayout il, Transform* t) : rawData(r), material(m), il(il), trans(t) {}

			inline bool operator< (const Instance& other) const noexcept
			{
				if (material->GetShader() < other.material->GetShader()) return true;
				else return rawData < other.rawData;
			}
		};

		std::vector<Instance> m_Instances;

	public:

		inline void Begin()
		{
			m_Instances.clear();
		}

		inline void Reserve(uint32 count)
		{
			m_Instances.reserve(count);
		}

		inline void Draw(Mesh* mesh, Transform* trans)
		{
			m_Instances.emplace_back(mesh->GetRawData(), mesh->GetMaterial(), mesh->GetInputLayout(), trans);
		}

		inline void End()
		{
			std::sort(m_Instances.begin(), m_Instances.end());
		}

		inline std::vector<Instance>& GetInstances() noexcept { return m_Instances; }

	};

}