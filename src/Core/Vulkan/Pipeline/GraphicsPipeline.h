#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace Core
{
	struct PushConstants
	{
		alignas(16) glm::mat4 model;
	};

	class GraphicsPipeline
	{
		public:
			GraphicsPipeline();
			GraphicsPipeline(const vk::raii::Device& device, 
							 const vk::Format& colorAttatchmentFormat,
							 const vk::Format& depthFormat,
							 const vk::raii::DescriptorSetLayout& descriptorSetLayout);
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

			[[nodiscard]] vk::raii::ShaderModule createShaderModule(const vk::raii::Device& device, const std::vector<char>& code) const;
	};
}