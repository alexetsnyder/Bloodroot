#pragma once

#include "IRenderable.h"
#include "Mesh.h"
#include "Voxel.h"

#include <glm/glm.hpp>

#include <memory>

namespace Game
{
	enum class CubeFace
	{
		LEFT,
		RIGHT,
		TOP,
		BOTTOM,
		FRONT,
		BACK,
	};

	class Chunk : public Core::IRenderable
	{
		public:
			const int VOXEL_SIZE = 1;

			Chunk(uint32_t width, uint32_t height, uint32_t depth, const glm::vec3& position);
			~Chunk();

			uint32_t Width() const { return width; }
			uint32_t Height() const { return height; }
			uint32_t Depth() const { return depth; }

			const glm::vec3& Position() const override { return position; }
			const Core::Mesh& Mesh() const { return mesh; }

			bool ShouldDraw() const { return shouldDraw; }

			bool IsInBounds(const glm::vec3& position) const;
			void CreateFace(CubeFace face, const glm::vec3& position, const Voxel& voxel);

			uint32_t IndexCount() const override { return indexCount; }
			std::shared_ptr<Core::IAllocation> Allocation() const override { return allocationPtr; }

			void SetAllocation(std::shared_ptr<Core::IAllocation> allocation) { allocationPtr = allocation; }

		private:
			uint32_t width, height, depth;
			glm::vec3 position;
			Core::Mesh mesh;
			bool shouldDraw = false;
			uint32_t indexCount = 0;
			std::shared_ptr<Core::IAllocation> allocationPtr = nullptr;
			
			void createFace(CubeFace face, const glm::i32vec3& cubePos, const Voxel& voxel);

			glm::i32vec3 mapToLocal(const glm::vec3& position) const;
			void generateVoxel(const glm::vec3& voxelPos, const Voxel& voxel);
	};
}