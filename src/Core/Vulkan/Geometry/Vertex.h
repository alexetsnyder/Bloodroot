#pragma once

#include <Vulkan/vulkan_raii.hpp>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 

namespace Core
{
	struct Vertex
	{
		glm::vec3 pos;
		glm::vec2 texCoord;

		static vk::VertexInputBindingDescription getBindingDescription()
		{
			return { .binding = 0, .stride = sizeof(Vertex), .inputRate = vk::VertexInputRate::eVertex };
		}

		static std::array<vk::VertexInputAttributeDescription, 2> getAttibuteDescriptions()
		{
			return { {
				   {.location = 0, .binding = 0, .format = vk::Format::eR32G32B32Sfloat, .offset = offsetof(Vertex, pos) },
				   {.location = 1, .binding = 0, .format = vk::Format::eR32G32Sfloat, .offset = offsetof(Vertex, texCoord) }
			} };
		}
	};

}