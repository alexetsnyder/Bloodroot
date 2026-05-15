#pragma once

#include <vulkan/vulkan_raii.hpp>

namespace Core
{
	class IAllocation
	{
		public:
			virtual VkDeviceSize Offset() const = 0;

			virtual ~IAllocation() {}
	};
}