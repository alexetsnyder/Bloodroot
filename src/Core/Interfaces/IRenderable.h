#pragma once

#include "IAllocation.h"

#include <glm/glm.hpp>
#include <vulkan/vulkan_raii.hpp>

#include <memory>

namespace Core
{
	class IRenderable
	{
		public:
			virtual const glm::vec3& Position() const = 0;
			virtual uint32_t IndexCount() const = 0;
			virtual std::shared_ptr<IAllocation> Allocation() const = 0;

			virtual ~IRenderable() {};
	};
}
