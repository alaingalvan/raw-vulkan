#include "VkUtils.h"

namespace raw
{

void findBestExtensions(const std::vector<vk::ExtensionProperties>& installed, const std::vector<const char*>& wanted, std::vector<const char*>& out)
{
	for (const char* const& w : wanted) {
		for (vk::ExtensionProperties const& i : installed) {
			if (std::string(i.extensionName).compare(w) == 0) {
				out.emplace_back(w);
				break;
			}
		}
	}
}

void findBestLayers(const std::vector<vk::LayerProperties>& installed, const std::vector<const char*>& wanted, std::vector<const char*>& out)
{
	for (const char* const& w : wanted) {
		for (vk::LayerProperties const& i : installed) {
			if (std::string(i.layerName).compare(w) == 0) {
				out.emplace_back(w);
				break;
			}
		}
	}
}

uint32_t getQueueIndex(vk::PhysicalDevice& physicalDevice, vk::QueueFlagBits flags)
{
	std::vector<vk::QueueFamilyProperties> queueProps = physicalDevice.getQueueFamilyProperties();

	for (size_t i = 0; i < queueProps.size(); ++i)
	{
		if (queueProps[i].queueFlags & flags) {
			return static_cast<uint32_t>(i);
		}
	}

	// Default queue index
	return 0;
}

uint32_t getMemoryTypeIndex(vk::PhysicalDevice& physicalDevice, uint32_t typeBits, vk::MemoryPropertyFlags properties)
{
	auto gpuMemoryProps = physicalDevice.getMemoryProperties();
	for (uint32_t i = 0; i < gpuMemoryProps.memoryTypeCount; i++)
	{
		if ((typeBits & 1) == 1)
		{
			if ((gpuMemoryProps.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}
		typeBits >>= 1;
	}
	return 0;
};

}