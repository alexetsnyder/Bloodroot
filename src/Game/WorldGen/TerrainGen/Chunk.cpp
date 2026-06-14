#include "Chunk.h"

#include "RLEncoding.h"

#include <vector>

namespace Game
{
	Chunk::Chunk()
		: position{{ 0.0f }}
	{
		
	}

	Chunk::Chunk(const glm::vec3& position)
		: position{ position }
	{
		
	}

	Chunk::Chunk(const Chunk& other)
		: position{ other.position }
	{
		
	}

	Chunk::Chunk(Chunk&& other) noexcept
		: position{ other.position }, voxels{ other.voxels }
	{
		
	}

	Chunk& Chunk::operator=(Chunk&& other) noexcept
	{
		if (this != &other)
		{
			position = other.position;
			voxels = other.voxels;

			other.position = glm::vec3(0.0f);
		}

		return *this;
	}

	Chunk& Chunk::operator=(const Chunk& other)
	{
		if (this != &other)
		{
			position = other.position;
			voxels = other.voxels;
		}

		return *this;
	}

	Chunk::~Chunk()
	{

	}

	glm::i32vec3 Chunk::MapToChunkId(const glm::vec3& voxelPos)
	{
		auto x = static_cast<int32_t>(std::floorf(voxelPos.x / CHUNK_WIDTH));
		auto y = static_cast<int32_t>(std::floorf(voxelPos.y / CHUNK_HEIGHT));
		auto z = static_cast<int32_t>(std::floorf(voxelPos.z / CHUNK_DEPTH));

		return { x, y, z };
	}

	glm::i32vec3 Chunk::ChunkId() const
	{
		return MapToChunkId(position);
	}

	const VoxelType Chunk::GetVoxelType(const glm::i32vec3& position) const
	{
		auto localPos = mapToLocal(position);

		int32_t index = (localPos.x * CHUNK_WIDTH) + localPos.z;
		return Core::Math::RLEncoding::GetData<VoxelType>(voxels[index], localPos.y);
	}

	bool Chunk::IsInBounds(const glm::vec3& position) const
	{
		auto localPos = mapToLocal(position);

		if ((localPos.y >= 0 && localPos.y < CHUNK_HEIGHT) &&
			(localPos.x >= 0 && localPos.x < CHUNK_WIDTH) &&
			(localPos.z >= 0 && localPos.z < CHUNK_DEPTH))
		{
			return true;
		}

		return false;
	}

	void Chunk::CreateFace(CubeFace face, const glm::vec3& position, const Voxel& voxel, Core::VK::Mesh& mesh) const
	{
		auto cubePos = mapToLocal(position);

		createFace(face, cubePos, voxel, mesh);
	}

	void Chunk::AddVoxelColumn(int32_t xPos, int32_t yPos, int32_t zPos, const std::vector<VoxelType>& voxelTypes)
	{
		auto localPos = mapToLocal({ xPos, yPos, zPos });

		int32_t index = (localPos.x * CHUNK_WIDTH) + localPos.z;

		Core::Math::RLEncoding::Encode<VoxelType>(voxelTypes, voxels[index]);
	}

	void Chunk::SetVoxel(const glm::i32vec3& position, VoxelType voxelType)
	{
		auto localPos = mapToLocal(position);

		int32_t index = (localPos.x * CHUNK_WIDTH) + localPos.z;

		auto decodedColumn = Core::Math::RLEncoding::Decode<VoxelType>(voxels[index]);

		decodedColumn[position.y] = voxelType;

		voxels[index].clear();

		Core::Math::RLEncoding::Encode<VoxelType>(decodedColumn, voxels[index]);
	}

	void Chunk::createFace(CubeFace face, const glm::i32vec3& cubePos, const Voxel& voxel, Core::VK::Mesh& mesh) const
	{
		int32_t x = cubePos.x;
		int32_t y = cubePos.y;
		int32_t z = cubePos.z;

		switch (face)
		{
			case CubeFace::LEFT:
				mesh.AddVertex({ { x, y + VOXEL_SIZE, z + VOXEL_SIZE }, { 0.0f, 0.0f, voxel.leftFaceIndex } });
				mesh.AddVertex({ { x, y + VOXEL_SIZE, z }, { 1.0f, 0.0f, voxel.leftFaceIndex } });
				mesh.AddVertex({ { x, y, z + VOXEL_SIZE }, { 0.0f, 1.0f, voxel.leftFaceIndex } });
				mesh.AddVertex({ { x, y, z }, { 1.0f, 1.0f, voxel.leftFaceIndex } });
				break;
			case CubeFace::RIGHT:
				mesh.AddVertex({ { x + VOXEL_SIZE, y + VOXEL_SIZE, z }, { 1.0f, 0.0f, voxel.rightFaceIndex } });
				mesh.AddVertex({ { x + VOXEL_SIZE, y + VOXEL_SIZE, z + VOXEL_SIZE }, { 0.0f, 0.0f, voxel.rightFaceIndex } });
				mesh.AddVertex({ { x + VOXEL_SIZE, y, z }, { 1.0f, 1.0f, voxel.rightFaceIndex } });
				mesh.AddVertex({ { x + VOXEL_SIZE, y, z + VOXEL_SIZE }, { 0.0f, 1.0f, voxel.rightFaceIndex } });
				break;
			case CubeFace::TOP:
				mesh.AddVertex({ { x + VOXEL_SIZE, y + VOXEL_SIZE, z }, { 1.0f, 1.0f, voxel.topFaceIndex } });
				mesh.AddVertex({ { x, y + VOXEL_SIZE, z }, { 0.0f, 1.0f, voxel.topFaceIndex } });
				mesh.AddVertex({ { x + VOXEL_SIZE, y + VOXEL_SIZE, z + VOXEL_SIZE }, { 1.0f, 0.0f, voxel.topFaceIndex } });
				mesh.AddVertex({ { x, y + VOXEL_SIZE, z + VOXEL_SIZE }, { 0.0f, 0.0f, voxel.topFaceIndex } });
				break;
			case CubeFace::BOTTOM:
				mesh.AddVertex({ { x, y, z }, { 0.0f, 1.0f, voxel.bottomFaceIndex } });
				mesh.AddVertex({ { x + VOXEL_SIZE, y, z }, { 1.0f, 1.0f, voxel.bottomFaceIndex } });
				mesh.AddVertex({ { x, y, z + VOXEL_SIZE }, { 0.0f, 0.0f, voxel.bottomFaceIndex } });
				mesh.AddVertex({ { x + VOXEL_SIZE, y, z + VOXEL_SIZE }, { 1.0f, 0.0f, voxel.bottomFaceIndex } });
				break;
				case CubeFace::BACK:
				mesh.AddVertex({ { x + VOXEL_SIZE, y + VOXEL_SIZE, z + VOXEL_SIZE }, { 0.0f, 0.0f, voxel.frontFaceIndex } });
				mesh.AddVertex({ { x, y + VOXEL_SIZE, z + VOXEL_SIZE }, { 1.0f, 0.0f, voxel.frontFaceIndex } });
				mesh.AddVertex({ { x + VOXEL_SIZE, y, z + VOXEL_SIZE }, { 0.0f, 1.0f, voxel.frontFaceIndex } });
				mesh.AddVertex({ { x, y, z + VOXEL_SIZE }, { 1.0f, 1.0f, voxel.frontFaceIndex } });
				break;
			case CubeFace::FRONT:
				mesh.AddVertex({ { x, y + VOXEL_SIZE, z }, { 1.0f, 0.0f, voxel.backFaceIndex } });
				mesh.AddVertex({ { x + VOXEL_SIZE, y + VOXEL_SIZE, z }, { 0.0f, 0.0f, voxel.backFaceIndex } });
				mesh.AddVertex({ { x, y, z }, { 1.0f, 1.0f, voxel.backFaceIndex } });
				mesh.AddVertex({ { x + VOXEL_SIZE, y, z }, { 0.0f, 1.0f, voxel.backFaceIndex } });
				break;
		}

		mesh.IndexCount() += 6;
	}

	glm::i32vec3 Chunk::mapToLocal(const glm::vec3& position) const
	{
		int32_t x = static_cast<int32_t>(std::floorf(position.x - this->position.x));
		int32_t y = static_cast<int32_t>(std::floorf(position.y - this->position.y));
		int32_t z = static_cast<int32_t>(std::floorf(position.z - this->position.z));

		return glm::i32vec3(x, y, z);
	}

	void Chunk::generateVoxel(const glm::vec3& voxelPos, const Voxel& voxel, Core::VK::Mesh& mesh)
	{
		uint32_t vertexCount = 0;

		uint32_t x = static_cast<uint32_t>(voxelPos.x);
		uint32_t y = static_cast<uint32_t>(voxelPos.y);
		uint32_t z = static_cast<uint32_t>(voxelPos.z);

		//auto voxel = getVoxel({ x, y, z });

		//Front Face
		mesh.AddVertex({ { x + VOXEL_SIZE, y + VOXEL_SIZE, z + VOXEL_SIZE }, { 0.0f, 0.0f, voxel.frontFaceIndex } });
		mesh.AddVertex({ { x, y + VOXEL_SIZE, z + VOXEL_SIZE }, { 1.0f, 0.0f, voxel.frontFaceIndex } });
		mesh.AddVertex({ { x + VOXEL_SIZE, y, z + VOXEL_SIZE }, { 0.0f, 1.0f, voxel.frontFaceIndex } });
		mesh.AddVertex({ { x, y, z + VOXEL_SIZE }, { 1.0f, 1.0f, voxel.frontFaceIndex } });
		
		//Back Face
		mesh.AddVertex({ { x, y + VOXEL_SIZE, z }, { 1.0f, 0.0f, voxel.backFaceIndex } });
		mesh.AddVertex({ { x + VOXEL_SIZE, y + VOXEL_SIZE, z }, { 0.0f, 0.0f, voxel.backFaceIndex } });
		mesh.AddVertex({ { x, y, z }, { 1.0f, 1.0f, voxel.backFaceIndex } });
		mesh.AddVertex({ { x + VOXEL_SIZE, y, z }, { 0.0f, 1.0f, voxel.backFaceIndex } });

		//Top Face
		mesh.AddVertex({ { x + VOXEL_SIZE, y + VOXEL_SIZE, z }, { 1.0f, 1.0f, voxel.topFaceIndex } });
		mesh.AddVertex({ { x, y + VOXEL_SIZE, z }, { 0.0f, 1.0f, voxel.topFaceIndex } });
		mesh.AddVertex({ { x + VOXEL_SIZE, y + VOXEL_SIZE, z + VOXEL_SIZE }, { 1.0f, 0.0f, voxel.topFaceIndex } });
		mesh.AddVertex({ { x, y + VOXEL_SIZE, z + VOXEL_SIZE }, { 0.0f, 0.0f, voxel.topFaceIndex } });

		//Bottom Face
		mesh.AddVertex({ { x, y, z }, { 0.0f, 1.0f, voxel.bottomFaceIndex } });
		mesh.AddVertex({ { x + VOXEL_SIZE, y, z }, { 1.0f, 1.0f, voxel.bottomFaceIndex } });
		mesh.AddVertex({ { x, y, z + VOXEL_SIZE }, { 0.0f, 0.0f, voxel.bottomFaceIndex } });
		mesh.AddVertex({ { x + VOXEL_SIZE, y, z + VOXEL_SIZE }, { 1.0f, 0.0f, voxel.bottomFaceIndex } });

		//Left Face
		mesh.AddVertex({ { x, y + VOXEL_SIZE, z + VOXEL_SIZE }, { 0.0f, 0.0f, voxel.leftFaceIndex } });
		mesh.AddVertex({ { x, y + VOXEL_SIZE, z }, { 1.0f, 0.0f, voxel.leftFaceIndex } });
		mesh.AddVertex({ { x, y, z + VOXEL_SIZE }, { 0.0f, 1.0f, voxel.leftFaceIndex } });
		mesh.AddVertex({ { x, y, z }, { 1.0f, 1.0f, voxel.leftFaceIndex } });

		//Right Face
		mesh.AddVertex({ { x + VOXEL_SIZE, y + VOXEL_SIZE, z }, { 1.0f, 0.0f, voxel.rightFaceIndex } });
		mesh.AddVertex({ { x + VOXEL_SIZE, y + VOXEL_SIZE, z + VOXEL_SIZE }, { 0.0f, 0.0f, voxel.rightFaceIndex } });
		mesh.AddVertex({ { x + VOXEL_SIZE, y, z }, { 1.0f, 1.0f, voxel.rightFaceIndex } });
		mesh.AddVertex({ { x + VOXEL_SIZE, y, z + VOXEL_SIZE }, { 0.0f, 1.0f, voxel.rightFaceIndex } });
		
		for (int i = 0; i < 6; i++)
		{
			mesh.AddIndex(vertexCount);
			mesh.AddIndex(vertexCount + 1);
			mesh.AddIndex(vertexCount + 2);

			mesh.AddIndex(vertexCount + 2);
			mesh.AddIndex(vertexCount + 1);
			mesh.AddIndex(vertexCount + 3);

			vertexCount += 4;
			mesh.IndexCount() += 6;
		}
	}
}