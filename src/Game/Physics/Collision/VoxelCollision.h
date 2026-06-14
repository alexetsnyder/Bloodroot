#pragma once

#include "GLMExtensions.h"
#include "Voxel.h"

#include <glm/glm.hpp>

#include <iostream>

namespace Game::PHYS
{
	class VoxelCollision
	{
		public:
			VoxelCollision() {}
			VoxelCollision(const Game::VoxelType& voxelType, const glm::i32vec3& position, const glm::i32vec3& normal)
			{
				this->voxelType = voxelType;
				this->position = position;
				this->normal = normal;
			}

			Game::VoxelType VoxelType() const { return voxelType; }
			const glm::i32vec3& Position() const { return position; }
			const glm::i32vec3& Normal() const { return normal; }

		private:
			Game::VoxelType voxelType;
			glm::i32vec3 position;
			glm::i32vec3 normal;
	};

	inline std::ostream& operator<<(std::ostream& os, const VoxelCollision& collision)
	{
		os << "VoxelType: " << collision.VoxelType();
		
		os << " Position: ";
		Core::Ext::operator<<(os, collision.Position());

		os << " Normal: ";
		Core::Ext::operator<<(os, collision.Normal());

		return os;
	}
}