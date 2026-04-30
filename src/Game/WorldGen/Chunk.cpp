#include "Chunk.h"

namespace Game
{
	Chunk::Chunk(int width, int height, int depth, glm::vec3 position)
	{
		this->width = width;
		this->height = height;
		this->depth = depth;
		this->position = position;
	}

	Chunk::~Chunk()
	{

	}

	void Chunk::generateMesh()
	{
		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				for (int z = 0; z < depth; z++)
				{
					generateVoxel(x, y, z);
				}
			}
		}
	}

	void Chunk::generateVoxel(int x, int y, int z)
	{
		int voxelSize = 1;

		//Front Face
		mesh.AddVertex({ { x + voxelSize, y + voxelSize, z + voxelSize }, { 0.0f, 0.0f, 1.0f } });
		mesh.AddVertex({ { x, y + voxelSize, z + voxelSize }, { 1.0f, 0.0f, 1.0f } });
		mesh.AddVertex({ { x + voxelSize, y, z + voxelSize }, { 0.0f, 1.0f, 1.0f } });
		mesh.AddVertex({ { x, y, z + voxelSize }, { 1.0f, 1.0f, 1.0f } });
		
		//Back Face
		mesh.AddVertex({ { x, y + voxelSize, z }, { 1.0f, 0.0f, 1.0f } });
		mesh.AddVertex({ { x + voxelSize, y + voxelSize, z }, { 0.0f, 0.0f, 1.0f } });
		mesh.AddVertex({ { x, y, z }, { 1.0f, 1.0f, 1.0f  } });
		mesh.AddVertex({ { x + voxelSize, y, z }, { 0.0f, 1.0f, 1.0f } });

		//Top Face
		mesh.AddVertex({ { x + voxelSize, y + voxelSize, z }, { 1.0f, 1.0f, 2.0f } });
		mesh.AddVertex({ { x, y + voxelSize, z }, { 0.0f, 1.0f, 2.0f } });
		mesh.AddVertex({ { x + voxelSize, y + voxelSize, z + voxelSize }, { 1.0f, 0.0f, 2.0f  } });
		mesh.AddVertex({ { x, y + voxelSize, z + voxelSize }, { 0.0f, 0.0f, 2.0f } });

		//Bottom Face
		mesh.AddVertex({ { x, y, z }, { 0.0f, 1.0f, 0.0f } });
		mesh.AddVertex({ { x + voxelSize, y, z }, { 1.0f, 1.0f, 0.0f } });
		mesh.AddVertex({ { x, y, z + voxelSize }, { 0.0f, 0.0f, 0.0f  } });
		mesh.AddVertex({ { x + voxelSize, y, z + voxelSize }, { 1.0f, 0.0f, 0.0f } });

		//Left Face
		mesh.AddVertex({ { x, y + voxelSize, z + voxelSize }, { 0.0f, 0.0f, 1.0f } });
		mesh.AddVertex({ { x, y + voxelSize, z }, { 1.0f, 0.0f, 1.0f } });
		mesh.AddVertex({ { x, y, z + voxelSize }, { 0.0f, 1.0f, 1.0f  } });
		mesh.AddVertex({ { x, y, z }, { 1.0f, 1.0f, 1.0f } });

		//Right Face
		mesh.AddVertex({ { x + voxelSize, y + voxelSize, z }, { 1.0f, 0.0f, 1.0f } });
		mesh.AddVertex({ { x + voxelSize, y + voxelSize, z + voxelSize }, { 0.0f, 0.0f, 1.0f } });
		mesh.AddVertex({ { x + voxelSize, y, z }, { 1.0f, 1.0f, 1.0f  } });
		mesh.AddVertex({ { x + voxelSize, y, z + voxelSize }, { 0.0f, 1.0f, 1.0f } });
		
		int index = 0;

		for (int i = 0; i < 6; i++)
		{
			mesh.AddIndex(index);
			mesh.AddIndex(index + 1);
			mesh.AddIndex(index + 2);

			mesh.AddIndex(index + 2);
			mesh.AddIndex(index + 1);
			mesh.AddIndex(index + 3);

			index += 4;
		}
	}
}