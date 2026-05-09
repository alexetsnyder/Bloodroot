#include "Chunk.h"

#include <vector>

namespace Game
{
	Chunk::Chunk(uint32_t width, uint32_t height, uint32_t depth, const glm::vec3& position)
	{
		this->width = width;
		this->height = height;
		this->depth = depth;
		this->position = position;
	}

	Chunk::~Chunk()
	{

	}

	bool Chunk::IsInBounds(const glm::vec3& position) const
	{
		auto localPos = mapToLocal(position);

		if ((localPos.y >= 0 && localPos.y < height) &&
			(localPos.x >= 0 && localPos.x < width) &&
			(localPos.z >= 0 && localPos.z < depth))
		{
			return true;
		}

		return false;
	}

	void Chunk::CreateFace(CubeFace face, const glm::vec3& position, const Voxel& voxel, Core::Mesh& mesh, int& vertexCount) const
	{
		//auto cubePos = mapToLocal(position);

		int32_t x = static_cast<int>(std::floorf(position.x));
		int32_t y = static_cast<int>(std::floorf(position.y));
		int32_t z = static_cast<int>(std::floorf(position.z));

		createFace(face, { x, y, z }, voxel, mesh, vertexCount);
	}

	void Chunk::createFace(CubeFace face, const glm::i32vec3& cubePos, const Voxel& voxel, Core::Mesh& mesh, int& vertexCount) const
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
			case CubeFace::FRONT:
				mesh.AddVertex({ { x + VOXEL_SIZE, y + VOXEL_SIZE, z + VOXEL_SIZE }, { 0.0f, 0.0f, voxel.frontFaceIndex } });
				mesh.AddVertex({ { x, y + VOXEL_SIZE, z + VOXEL_SIZE }, { 1.0f, 0.0f, voxel.frontFaceIndex } });
				mesh.AddVertex({ { x + VOXEL_SIZE, y, z + VOXEL_SIZE }, { 0.0f, 1.0f, voxel.frontFaceIndex } });
				mesh.AddVertex({ { x, y, z + VOXEL_SIZE }, { 1.0f, 1.0f, voxel.frontFaceIndex } });
				break;
			case CubeFace::BACK:
				mesh.AddVertex({ { x, y + VOXEL_SIZE, z }, { 1.0f, 0.0f, voxel.backFaceIndex } });
				mesh.AddVertex({ { x + VOXEL_SIZE, y + VOXEL_SIZE, z }, { 0.0f, 0.0f, voxel.backFaceIndex } });
				mesh.AddVertex({ { x, y, z }, { 1.0f, 1.0f, voxel.backFaceIndex } });
				mesh.AddVertex({ { x + VOXEL_SIZE, y, z }, { 0.0f, 1.0f, voxel.backFaceIndex } });
				break;
		}

		mesh.AddIndex(vertexCount);
		mesh.AddIndex(vertexCount + 1);
		mesh.AddIndex(vertexCount + 2);

		mesh.AddIndex(vertexCount + 2);
		mesh.AddIndex(vertexCount + 1);
		mesh.AddIndex(vertexCount + 3);

		vertexCount += 4;
	}

	glm::i32vec3 Chunk::mapToLocal(const glm::vec3& position) const
	{
		int32_t x = static_cast<int32_t>(std::floorf(position.x - this->position.x));
		int32_t y = static_cast<int32_t>(std::floorf(position.y - this->position.y));
		int32_t z = static_cast<int32_t>(std::floorf(position.z - this->position.z));

		return glm::i32vec3(x, y, z);
	}

	void Chunk::generateVoxel(const glm::vec3& voxelPos, const Voxel& voxel, Core::Mesh& mesh, int& vertexCount)
	{
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
		}
	}
}