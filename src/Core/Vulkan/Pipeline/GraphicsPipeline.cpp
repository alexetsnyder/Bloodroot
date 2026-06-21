#include "GraphicsPipeline.h"

#include "Vertex.h"
#include "VulkanHandles.h"

namespace Core::VK
{
	GraphicsPipeline::GraphicsPipeline()
		: pipelineLayout{ nullptr }, graphicsPipeline{ nullptr }
	{

	}

	GraphicsPipeline::GraphicsPipeline(const ShaderModule& shaderModule,
									   const vk::Format& colorAttatchmentFormat,
									   const vk::Format& depthFormat,
									   const vk::raii::DescriptorSetLayout& descriptorSetLayout,
									   vk::Bool32 enableDepthTest,
									   vk::Bool32 enableDepthWrite,
									   vk::Bool32 enableColorBlending,
									   vk::CullModeFlags cullMode)
	{
		vk::PipelineColorBlendAttachmentState colorBlendAttachment;
		if (enableColorBlending == vk::True)
		{
			colorBlendAttachment = 
			{
				.blendEnable = vk::True,
				.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha,
				.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha,
				.colorBlendOp = vk::BlendOp::eAdd,
				.srcAlphaBlendFactor = vk::BlendFactor::eOne,
				.dstAlphaBlendFactor = vk::BlendFactor::eZero,
				.alphaBlendOp = vk::BlendOp::eAdd,
				.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
			};
		}
		else
		{
			colorBlendAttachment =
			{
				.blendEnable = vk::False,
				.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
			};
		}

		vk::PipelineDepthStencilStateCreateInfo depthStateCreateInfo
		{
			.depthTestEnable = enableDepthTest,
			.depthWriteEnable = enableDepthWrite,
			.depthCompareOp = vk::CompareOp::eLess,
			.depthBoundsTestEnable = vk::False,
			.stencilTestEnable = vk::False
		};

		vk::PipelineRasterizationStateCreateInfo rasterizationStateCreateInfo
		{
			.depthClampEnable = vk::False,
			.rasterizerDiscardEnable = vk::False,
			.polygonMode = vk::PolygonMode::eFill,
			.cullMode = cullMode,
			.frontFace = vk::FrontFace::eCounterClockwise,
			.depthBiasEnable = vk::False,
			.lineWidth = 1.0f
		};

		createPipeline(
			shaderModule,
			colorAttatchmentFormat,
			depthFormat,
			descriptorSetLayout,
			colorBlendAttachment,
			depthStateCreateInfo,
			rasterizationStateCreateInfo
		);
	}

	GraphicsPipeline::GraphicsPipeline(const ShaderModule& shaderModule,
									   const vk::Format& colorAttatchmentFormat,
									   const vk::Format& depthFormat,
									   const vk::raii::DescriptorSetLayout& descriptorSetLayout,
									   const vk::PipelineColorBlendAttachmentState& colorBlendAttachment,
									   const vk::PipelineDepthStencilStateCreateInfo& depthStateCreateInfo)
	{
		vk::PipelineRasterizationStateCreateInfo rasterizer
		{
			.depthClampEnable = vk::False,
			.rasterizerDiscardEnable = vk::False,
			.polygonMode = vk::PolygonMode::eFill,
			.cullMode = vk::CullModeFlagBits::eBack,
			.frontFace = vk::FrontFace::eCounterClockwise,
			.depthBiasEnable = vk::False,
			.lineWidth = 1.0f
		};

		createPipeline(
			shaderModule,
			colorAttatchmentFormat,
			depthFormat,
			descriptorSetLayout,
			colorBlendAttachment,
			depthStateCreateInfo,
			rasterizer
		);
	}

	GraphicsPipeline::GraphicsPipeline(GraphicsPipeline&& other) noexcept
	{
		pipelineLayout = std::move(other.pipelineLayout);
		graphicsPipeline = std::move(other.graphicsPipeline);

		other.pipelineLayout = nullptr;
		other.graphicsPipeline = nullptr;
	}

	GraphicsPipeline& GraphicsPipeline::operator=(GraphicsPipeline&& other) noexcept
	{
		if (this != &other)
		{
			pipelineLayout = std::move(other.pipelineLayout);
			graphicsPipeline = std::move(other.graphicsPipeline);

			other.pipelineLayout = nullptr;
			other.graphicsPipeline = nullptr;
		}

		return *this;
	}

	GraphicsPipeline::~GraphicsPipeline()
	{

	}

	void GraphicsPipeline::BindDescriptorSets(const vk::raii::CommandBuffer& commandBuffer, const vk::raii::DescriptorSet& descriptorSet)
	{
		commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, *descriptorSet, nullptr);
	}

	void GraphicsPipeline::BindPipeline(const vk::raii::CommandBuffer& commandBuffer) const
	{
		commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline);
	}

	void GraphicsPipeline::SendPushConstants(const vk::raii::CommandBuffer& commandBuffer, const PushConstants& pushConstants) const
	{
		commandBuffer.pushConstants<PushConstants>(
			pipelineLayout,
			vk::ShaderStageFlagBits::eVertex,
			0,
			pushConstants
		);
	}

	void GraphicsPipeline::createPipeline(const ShaderModule& shaderModule, const vk::Format& colorAttatchmentFormat, const vk::Format& depthFormat, const vk::raii::DescriptorSetLayout& descriptorSetLayout, const vk::PipelineColorBlendAttachmentState& colorBlendAttachment, const vk::PipelineDepthStencilStateCreateInfo& depthStateCreateInfo, const vk::PipelineRasterizationStateCreateInfo& rasterizorCreateInfo)
	{
		auto shaderStages = shaderModule.GetShaderStages();

		const auto& device = VulkanHandles::Instance().Device();

		auto bindingDescription = Vertex::getBindingDescription();
		auto attibuteDescriptions = Vertex::getAttibuteDescriptions();
		vk::PipelineVertexInputStateCreateInfo vertexInputInfo
		{
			.vertexBindingDescriptionCount = 1,
			.pVertexBindingDescriptions = &bindingDescription,
			.vertexAttributeDescriptionCount = static_cast<uint32_t>(attibuteDescriptions.size()),
			.pVertexAttributeDescriptions = attibuteDescriptions.data()
		};

		vk::PipelineInputAssemblyStateCreateInfo inputAssembly{ .topology = vk::PrimitiveTopology::eTriangleList };
		vk::PipelineViewportStateCreateInfo viewPortState{ .viewportCount = 1, .scissorCount = 1 };

		vk::PipelineMultisampleStateCreateInfo multisampling{ .rasterizationSamples = vk::SampleCountFlagBits::e1, .sampleShadingEnable = vk::False };

		vk::PipelineColorBlendStateCreateInfo colorBlending
		{
			.logicOpEnable = vk::False,
			.logicOp = vk::LogicOp::eCopy,
			.attachmentCount = 1,
			.pAttachments = &colorBlendAttachment
		};

		std::vector<vk::DynamicState> dynamicStates 
		{ 
			vk::DynamicState::eViewport, 
			vk::DynamicState::eScissor 
		};

		vk::PipelineDynamicStateCreateInfo dynamicState
		{ 
			.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
			.pDynamicStates = dynamicStates.data()
		};

		vk::PushConstantRange pushConstantRange
		{
			.stageFlags = vk::ShaderStageFlagBits::eVertex,
			.offset = 0,
			.size = sizeof(PushConstants),
		};

		vk::PipelineLayoutCreateInfo pipelineLayoutInfo
		{
			.setLayoutCount = 1,
			.pSetLayouts = &*descriptorSetLayout,
			.pushConstantRangeCount = 1,
			.pPushConstantRanges = &pushConstantRange,
		};

		pipelineLayout = vk::raii::PipelineLayout(device, pipelineLayoutInfo);

		vk::StructureChain<vk::GraphicsPipelineCreateInfo, vk::PipelineRenderingCreateInfo> pipelineCreateInfoChain =
		{
			{
				.stageCount = 2,
				.pStages = shaderStages.data(),
				.pVertexInputState = &vertexInputInfo,
				.pInputAssemblyState = &inputAssembly,
				.pViewportState = &viewPortState,
				.pRasterizationState = &rasterizorCreateInfo,
				.pMultisampleState = &multisampling,
				.pDepthStencilState = &depthStateCreateInfo,
				.pColorBlendState = &colorBlending,
				.pDynamicState = &dynamicState,
				.layout = pipelineLayout,
				.renderPass = nullptr
			},
			{
				.colorAttachmentCount = 1,
				.pColorAttachmentFormats = &colorAttatchmentFormat,
				.depthAttachmentFormat = depthFormat
			}
		};

		graphicsPipeline = vk::raii::Pipeline(device, nullptr, pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>());
	}
}