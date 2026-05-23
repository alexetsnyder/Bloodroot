#pragma once

#include "Mesh.h"
#include "Voxel.h"

#include <glm/glm.hpp>

#include <array>
#include <cstdint>
#include <vector>

namespace Game
{
	constexpr uint16_t VOXEL_COUNT = 5;
	constexpr uint32_t WIDTH = 16;
	constexpr uint32_t HEIGHT = 256;
	constexpr uint32_t DEPTH = 16;

	enum class CubeFace
	{
		LEFT,
		RIGHT,
		TOP,
		BOTTOM,
		FRONT,
		BACK,
	};

	class Chunk
	{
		public:
			const int VOXEL_SIZE = 1;
			static uint32_t NextId;

			Chunk();
			Chunk(uint32_t width, uint32_t height, uint32_t depth, const glm::vec3& position);
			Chunk(const Chunk& other);
			Chunk(Chunk&& other) noexcept;
			Chunk& operator=(const Chunk& other);
			~Chunk();

			uint32_t Width() const { return width; }
			uint32_t Height() const { return height; }
			uint32_t Depth() const { return depth; }
			glm::u32vec3 ChunkSize() const { return { width, height, depth }; }

			const glm::vec3& Position() const { return position; }
			glm::i32vec3 ChunkId() const;
			const uint32_t UniqueId() const { return uniqueId; }

			bool IsInBounds(const glm::vec3& position) const;
			void CreateFace(CubeFace face, const glm::vec3& position, const Voxel& voxel, Core::Mesh& mesh) const;

		private:
			uint32_t uniqueId;
			uint32_t width, height, depth;
			glm::vec3 position;
			std::array<std::vector<uint16_t>, WIDTH * DEPTH> voxels;
			
			void init(uint32_t width, uint32_t height, uint32_t depth, const glm::vec3& position);

			void createFace( CubeFace face, const glm::i32vec3& cubePos, const Voxel& voxel, Core::Mesh& mesh) const;

			glm::i32vec3 mapToLocal(const glm::vec3& position) const;
			void generateVoxel(const glm::vec3& voxelPos, const Voxel& voxel, Core::Mesh& mesh);
	};
}