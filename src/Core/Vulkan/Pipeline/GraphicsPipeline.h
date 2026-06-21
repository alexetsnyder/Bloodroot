#pragma once

#include "ShaderModule.h"

#include <glm/glm.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace Core::VK
{
	struct PushConstants
	{
		alignas(16) glm::mat4 model;
		alignas(16) glm::mat4 projection;
	};

	class GraphicsPipeline
	{
		public:
			GraphicsPipeline();
			GraphicsPipeline(const ShaderModule& shaderModule,
							 const vk::Format& colorAttatchmentFormat,
							 const vk::Format& depthFormat,
							 const vk::raii::DescriptorSetLayout& descriptorSetLayout,
							 vk::Bool32 enableDepthTest,
							 vk::Bool32 enableDepthWrite,
							 vk::Bool32 enableColorBlending,
							 vk::CullModeFlags cullMode);
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

			void BindDescriptorSets(const vk::raii::CommandBuffer& commandBuffer, const vk::raii::DescriptorSet& descriptorSet);
			void BindPipeline(const vk::raii::CommandBuffer& commandBuffer) const;
			void SendPushConstants(const vk::raii::CommandBuffer& commandBuffer, const PushConstants& pushConstants) const;

		private:
			vk::raii::PipelineLayout pipelineLayout = nullptr;
			vk::raii::Pipeline graphicsPipeline = nullptr;

			void createPipeline(const ShaderModule& shaderModule,
								const vk::Format& colorAttatchmentFormat,
								const vk::Format& depthFormat,
								const vk::raii::DescriptorSetLayout& descriptorSetLayout,
								const vk::PipelineColorBlendAttachmentState& colorBlendAttachment,
								const vk::PipelineDepthStencilStateCreateInfo& depthStateCreateInfo,
								const vk::PipelineRasterizationStateCreateInfo& rasterizorCreateInfo);
	};
}