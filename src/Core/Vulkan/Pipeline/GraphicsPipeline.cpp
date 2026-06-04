#include "GraphicsPipeline.h"

#include "FileIO.h"
#include "Vertex.h"

namespace Core::VK
{
	GraphicsPipeline::GraphicsPipeline()
		: pipelineLayout{ nullptr }, graphicsPipeline{ nullptr }
	{

	}

	GraphicsPipeline::GraphicsPipeline(const vk::raii::Device& device,
									   const vk::Format& colorAttatchmentFormat,
									   const vk::Format& depthFormat,
									   const vk::raii::DescriptorSetLayout& descriptorSetLayout,
									   const vk::PipelineColorBlendAttachmentState& colorBlendAttachment,
									   const vk::PipelineDepthStencilStateCreateInfo& depthStateCreateInfo)
	{
		vk::raii::ShaderModule shaderModule = createShaderModule(device, FileIO::readFile("Shaders/slang.spv"));

		vk::PipelineShaderStageCreateInfo vertShaderStageInfo{ .stage = vk::ShaderStageFlagBits::eVertex, .module = shaderModule, .pName = "vertMain" };
		vk::PipelineShaderStageCreateInfo fragShaderStageInfo{ .stage = vk::ShaderStageFlagBits::eFragment, .module = shaderModule, .pName = "fragMain" };
		vk::PipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

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
				.pStages = shaderStages,
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

	vk::raii::ShaderModule GraphicsPipeline::createShaderModule(const vk::raii::Device& device, const std::vector<char>& code) const
	{
		vk::ShaderModuleCreateInfo createInfo
		{ 
			.codeSize = code.size() * sizeof(char),
			.pCode = reinterpret_cast<const uint32_t*>(code.data()) 
		};

		vk::raii::ShaderModule shaderModule{ device, createInfo };

		return shaderModule;
	}
}