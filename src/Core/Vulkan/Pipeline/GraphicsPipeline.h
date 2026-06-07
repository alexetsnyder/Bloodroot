#pragma once

#include "ShaderModule.h"

#include <glm/glm.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace Core::VK
{
	struct PushConstants
	{
		alignas(16) glm::mat4 model;
	};

	class GraphicsPipeline
	{
		public:
			GraphicsPipeline();
			GraphicsPipeline(const ShaderModule& shaderModule, 
							 const vk::Format& colorAttatchmentFormat,
							 const vk::Format& depthFormat,
							 const vk::raii::DescriptorSetLayout& descriptorSetLayout,
							 const vk::PipelineColorBlendAttachmentState& colorBlendAttachment,
							 const vk::PipelineDepthStencilStateCreateInfo& depthStateCreateInfo);
			GraphicsPipeline(GraphicsPipeline&& other) noexcept;
			GraphicsPipeline& operator=(GraphicsPipeline&& other) noexcept;

			GraphicsPipeline(const GraphicsPipeline&) = delete;
			GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;

			~GraphicsPipeline();

			vk::raii::PipelineLayout& PipelineLayout() { return pipelineLayout; }
			vk::raii::Pipeline& Pipeline() { return graphicsPipeline; }

		private:
			vk::raii::PipelineLayout pipelineLayout = nullptr;
			vk::raii::Pipeline graphicsPipeline = nullptr;
	};
}