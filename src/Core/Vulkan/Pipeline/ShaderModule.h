#pragma once

#include "FileIO.h"
#include "VulkanHandles.h"

#include <vulkan/vulkan_raii.hpp>

#include <array>
#include <string>
#include <utility>

namespace Core::VK
{
	class ShaderModule
	{
		public:
			ShaderModule(std::nullptr_t)
			{

			}

			ShaderModule(const std::string& filePath)
			{
				auto code = FileIO::readFile(filePath);

				vk::ShaderModuleCreateInfo createInfo
				{
					.codeSize = code.size() * sizeof(char),
					.pCode = reinterpret_cast<const uint32_t*>(code.data())
				};

				shaderModule = vk::raii::ShaderModule{ VulkanHandles::Instance().Device(), createInfo};
			}

			ShaderModule(const ShaderModule&) = delete;
			ShaderModule& operator=(const ShaderModule&) = delete;

			ShaderModule(ShaderModule&& other) noexcept
			{
				shaderModule = std::move(other.shaderModule);
			}

			ShaderModule& operator=(ShaderModule&& other) noexcept
			{
				if (this != &other)
				{
					shaderModule = std::move(other.shaderModule);
				}

				return *this;
			}

			~ShaderModule()
			{

			}

			std::array<vk::PipelineShaderStageCreateInfo, 2> GetShaderStages() const
			{
				vk::PipelineShaderStageCreateInfo vertShaderStageInfo
				{
					.stage = vk::ShaderStageFlagBits::eVertex,
					.module = shaderModule,
					.pName = "vertMain"
				};

				vk::PipelineShaderStageCreateInfo fragShaderStageInfo
				{
					.stage = vk::ShaderStageFlagBits::eFragment,
					.module = shaderModule,
					.pName = "fragMain"
				};

				return { vertShaderStageInfo, fragShaderStageInfo };
			}

		private:
			vk::raii::ShaderModule shaderModule = nullptr;
	};
}