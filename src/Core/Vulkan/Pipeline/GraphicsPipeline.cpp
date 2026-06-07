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
									   const vk::PipelineColorBlendAttachmentState& colorBlendAttachment,
									   const vk::PipelineDepthStencilStateCreateInfo& depthStateCreateInfo)
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

		vk::PipelineMultisampleStateCreateInfo multisampling{ .rasterizationSamples = vk::SampleCountFlagBits::e1, .sampleShadingEnable = vk::False };

		vk::PipelineColorBlendStateCreateInfo colorBlending
		{
			.logicOpEnable = vk::False,
			.logicOp = vk::LogicOp::eCopy,
			.attachmentCount = 1,
			.pAttachments = &colorBlendAttachment
		};

		std::vector<vk::DynamicState> dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
		vk::PipelineDynamicStateCreateInfo dynamicState{ .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()), .pDynamicStates = dynamicStates.data() };

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
				.pRasterizationState = &rasterizer,
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

	GraphicsPipeline::GraphicsPipeline(GraphicsPipeline && other) noexcept
	{
		pipelineLayout = std::move(other.pipelineLayout);
		graphicsPipeline = std::move(other.graphicsPipeline);

		other.pipelineLayout = nullptr;
		other.graphicsPipeline = nullptr;
	}

	GraphicsPipeline& GraphicsPipeline::operator=(GraphicsPipeline && other) noexcept
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
}