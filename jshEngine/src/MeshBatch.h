#pragma once

#include "Mesh.h"
#include <vector>

namespace jsh {

	struct Transform;

	struct MeshInstance {
		Mesh* mesh;
		Transform* pTransform;

		MeshInstance(Mesh* mesh, Transform* transform)
			: mesh(mesh), pTransform(transform) {}

		bool operator<(const MeshInstance& other)
		{
			if (mesh->IsTransparent()) {
				if (other.mesh->IsTransparent()) return mesh < other.mesh;
				else return true;
			}
			else {
				if (other.mesh->IsTransparent()) return false;
				else return mesh < other.mesh;
			}
		}
	};

	class MeshBatch {		
		std::vector<MeshInstance> m_Instances;

	public:
		inline void Begin() noexcept
		{
			m_Instances.clear();
		}

		inline void ReserveInstances(uint32 count) noexcept
		{
			m_Instances.reserve(count);
		}

		inline void Draw(Transform* pTransform, Mesh* pMesh) noexcept
		{
			m_Instances.emplace_back(MeshInstance(pMesh, pTransform));
		}
		inline std::vector<MeshInstance>& GetInstances() noexcept
		{
			return m_Instances;
		}

	};

}