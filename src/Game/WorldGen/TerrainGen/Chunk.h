#pragma once

#include "Mesh.h"
#include "Voxel.h"

#include <glm/glm.hpp>

#include <array>
#include <cstdint>
#include <vector>

namespace Game
{
	constexpr uint32_t CHUNK_WIDTH = 16;
	constexpr uint32_t CHUNK_HEIGHT = 256;
	constexpr uint32_t CHUNK_DEPTH = 16;

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

			Chunk();
			Chunk(const glm::vec3& position);
			Chunk(const Chunk& other);
			Chunk(Chunk&& other) noexcept;
			Chunk& operator=(Chunk&& other) noexcept;
			Chunk& operator=(const Chunk& other);
			~Chunk();

			static glm::i32vec3 MapToChunkId(const glm::vec3& voxelPos);
			const glm::vec3& Position() const { return position; }
			glm::i32vec3 ChunkId() const;
			const VoxelType GetVoxelType(const glm::i32vec3& position) const;

			bool IsInBounds(const glm::vec3& position) const;
			void CreateFace(CubeFace face, const glm::vec3& position, const Voxel& voxel, Core::VK::Mesh& mesh) const;
			void AddVoxelColumn(int32_t xPos, int32_t yPos, int32_t zPos, const std::vector<VoxelType>& voxelTypes);

		private:
			glm::vec3 position;
			std::array<std::vector<uint16_t>, CHUNK_WIDTH * CHUNK_DEPTH> voxels;

			void createFace(CubeFace face, const glm::i32vec3& cubePos, const Voxel& voxel, Core::VK::Mesh& mesh) const;

			glm::i32vec3 mapToLocal(const glm::vec3& position) const;
			void generateVoxel(const glm::vec3& voxelPos, const Voxel& voxel, Core::VK::Mesh& mesh);
	};
}