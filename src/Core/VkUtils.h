#pragma once

namespace raw
{

void findBestExtensions(const std::vector<vk::ExtensionProperties>& installed, const std::vector<const char*>& wanted, std::vector<const char*>& out);

void findBestLayers(const std::vector<vk::LayerProperties>& installed, const std::vector<const char*>& wanted, std::vector<const char*>& out);

uint32_t getQueueIndex(vk::PhysicalDevice& physicalDevice, vk::QueueFlagBits flags);

uint32_t getMemoryTypeIndex(vk::PhysicalDevice& physicalDevice, uint32_t typeBits, vk::MemoryPropertyFlags properties);

}