#include <fstream>
#include <vector>
#include <string>
#include <iostream>

#include "vulkan/vulkan.hpp"
#include "glm/glm.hpp"

int main()
{

#pragma region StartInstance

	auto installedExtensions = vk::enumerateInstanceExtensionProperties();

	std::vector<const char*> wantedExtensions =
	{
		VK_EXT_DEBUG_REPORT_EXTENSION_NAME
	};

	auto extensions = std::vector<const char*>();

	for (auto &w : wantedExtensions) {
		for (auto &i : installedExtensions) {
			if (std::string(i.extensionName).compare(w) == 0) {
				extensions.emplace_back(w);
				break;
			}
		}
	}

	auto installedLayers = vk::enumerateInstanceLayerProperties();

	std::vector<const char*> wantedLayers =
	{
		"VK_LAYER_LUNARG_standard_validation",
		"VK_LAYER_RENDERDOC_Capture"
	};

	auto layers = std::vector<const char*>();

	for (auto &w : wantedLayers) {
		for (auto &i : installedLayers) {
			if (std::string(i.layerName).compare(w) == 0) {
				layers.emplace_back(w);
				break;
			}
		}
	}

	auto appInfo = vk::ApplicationInfo(
		"BitmapRenderer",
		VK_MAKE_VERSION(1, 0, 0),
		"BitmapRendererEngine",
		VK_MAKE_VERSION(1, 0, 0),
		VK_API_VERSION_1_0
	);

	auto instance = vk::createInstance(
		vk::InstanceCreateInfo(
			vk::InstanceCreateFlags(),
			&appInfo,
			layers.size(),
			layers.data(),
			extensions.size(),
			extensions.data()
		)
	);

#pragma endregion

#pragma region PhysicalDevices

	// Initialize Devices
	auto physicalDevices = instance.enumeratePhysicalDevices();
	auto physicalDevice = physicalDevices[0];
	auto gpuMemoryProps = physicalDevice.getMemoryProperties();

#pragma endregion

#pragma region LogicalDevice
	auto physicalDeviceExtensions = physicalDevice.enumerateDeviceExtensionProperties();

	// Init Device Extension/Validation layers
	std::vector<const char*> wantedDeviceExtensions =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_EXT_DEBUG_MARKER_EXTENSION_NAME
	};

	auto deviceExtensions = std::vector<const char*>();

	for (auto &w : wantedDeviceExtensions) {
		for (auto &i : physicalDeviceExtensions) {
			if (std::string(i.extensionName).compare(w) == 0) {
				deviceExtensions.emplace_back(w);
				break;
			}
		}
	}

	auto physicalDeviceLayers = physicalDevice.enumerateDeviceLayerProperties();

	std::vector<const char*> wantedDeviceLayers =
	{
		"VK_LAYER_LUNARG_standard_validation",
		"VK_LAYER_RENDERDOC_Capture"
	};

	auto deviceLayers = std::vector<const char*>();

	for (auto &w : wantedLayers) {
		for (auto &i : installedLayers) {
			if (std::string(i.layerName).compare(w) == 0) {
				layers.emplace_back(w);
				break;
			}
		}
	}

	auto physicalDeviceFeatures = physicalDevice.getFeatures();
	auto physicalDeviceQueueProps = physicalDevice.getQueueFamilyProperties();

	float priority = 0.0;
	uint32_t graphicsFamilyIndex = 0;
	auto queueCreateInfos = std::vector<vk::DeviceQueueCreateInfo>();

	for (auto& queuefamily : physicalDeviceQueueProps)
	{
		if (queuefamily.queueFlags & vk::QueueFlagBits::eGraphics) {
			// Create a single graphics queue.
			queueCreateInfos.push_back(
				vk::DeviceQueueCreateInfo(
					vk::DeviceQueueCreateFlags(),
					graphicsFamilyIndex,
					1,
					&priority
				)
			);
			break;
		}

		graphicsFamilyIndex++;

	}

	auto logicalDevice = physicalDevice.createDevice(
		vk::DeviceCreateInfo(
			vk::DeviceCreateFlags(),
			queueCreateInfos.size(),
			queueCreateInfos.data(),
			deviceLayers.size(),
			deviceLayers.data(),
			deviceExtensions.size(),
			deviceExtensions.data(),
			&physicalDeviceFeatures
		)
	);

#pragma endregion

#pragma region Queue
	auto graphicsQueue = logicalDevice.getQueue(graphicsFamilyIndex, 0);
#pragma endregion

#pragma region RenderPass
	std::vector<vk::AttachmentDescription> attachmentDescriptions =
	{
		vk::AttachmentDescription(
			vk::AttachmentDescriptionFlags(),
			vk::Format::eB8G8R8A8Unorm,
			vk::SampleCountFlagBits::e1,
			vk::AttachmentLoadOp::eClear,
			vk::AttachmentStoreOp::eStore,
			vk::AttachmentLoadOp::eDontCare,
			vk::AttachmentStoreOp::eDontCare,
			vk::ImageLayout::eUndefined,
			vk::ImageLayout::ePresentSrcKHR
		)
	};

	std::vector<vk::AttachmentReference> colorReferences =
	{
		vk::AttachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal)
	};

	std::vector<vk::SubpassDescription> subpasses =
	{
		vk::SubpassDescription(
			vk::SubpassDescriptionFlags(),
			vk::PipelineBindPoint::eGraphics,
			0,
			nullptr,
			colorReferences.size(),
			colorReferences.data(),
			nullptr,
			nullptr,
			0,
			nullptr
		)
	};

	std::vector<vk::SubpassDependency> dependencies =
	{
		vk::SubpassDependency(
			~0U,
			0,
			vk::PipelineStageFlagBits::eBottomOfPipe,
			vk::PipelineStageFlagBits::eColorAttachmentOutput,
			vk::AccessFlagBits::eMemoryRead,
			vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite,
			vk::DependencyFlagBits::eByRegion
		)
	};

	auto renderpass = logicalDevice.createRenderPass(
		vk::RenderPassCreateInfo(
			vk::RenderPassCreateFlags(),
			attachmentDescriptions.size(),
			attachmentDescriptions.data(),
			subpasses.size(),
			subpasses.data(),
			dependencies.size(),
			dependencies.data()
		)
	);

#pragma endregion

#pragma region FrameBuffers
	// Output Image Attributes
	auto surfaceSize = vk::Extent2D(1280, 720);
	auto renderSize = vk::Rect2D(vk::Offset2D(), surfaceSize);
	auto viewport = vk::Viewport(0.0f, 0.0f, surfaceSize.width, surfaceSize.height, 0, 1.0f);

	// Create Output Image Data
	auto outputImage = logicalDevice.createImage(
		vk::ImageCreateInfo(
			vk::ImageCreateFlags(),
			vk::ImageType::e2D,
			vk::Format::eB8G8R8A8Unorm,
			vk::Extent3D(surfaceSize.width, surfaceSize.height, 1),
			1,
			1,
			vk::SampleCountFlagBits::e1,
			vk::ImageTiling::eOptimal,
			vk::ImageUsageFlagBits::eColorAttachment,
			vk::SharingMode::eExclusive,
			0,
			nullptr,
			vk::ImageLayout::eUndefined
		)
	);

	auto outputMemoryReq = logicalDevice.getImageMemoryRequirements(outputImage);

	// Lambda getMemoryTypeIndex(typebits, properties)
	// Search through GPU memory properies to see if this can be device local.
	auto getMemoryTypeIndex = [&](uint32_t typeBits, vk::MemoryPropertyFlags properties)
	{
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
	};

	// The Image is being rendered in 
	auto outputMemory = logicalDevice.allocateMemory(
		vk::MemoryAllocateInfo(
			outputMemoryReq.size,
			getMemoryTypeIndex(outputMemoryReq.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal)
		)
	);


	logicalDevice.bindImageMemory(
		outputImage,
		outputMemory,
		0
	);

	auto outputImageView = logicalDevice.createImageView(
		vk::ImageViewCreateInfo(
			vk::ImageViewCreateFlags(),
			outputImage,
			vk::ImageViewType::e2D,
			vk::Format::eB8G8R8A8Unorm,
			vk::ComponentMapping(),
			vk::ImageSubresourceRange(
				vk::ImageAspectFlagBits::eColor,
				0,
				1,
				0,
				1
			)
		)
	);

	auto outputFrameBuffer = logicalDevice.createFramebuffer(
		vk::FramebufferCreateInfo(
			vk::FramebufferCreateFlags(),
			renderpass,
			1,
			&outputImageView,
			surfaceSize.width,
			surfaceSize.height,
			1
		)
	);
#pragma endregion

#pragma region CommandPool

	auto commandPool = logicalDevice.createCommandPool(
		vk::CommandPoolCreateInfo(
			vk::CommandPoolCreateFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer),
			graphicsFamilyIndex
		)
	);

	// Allocate one buffer for each frame in the Swapchain.
	auto commandBuffers = logicalDevice.allocateCommandBuffers(
		vk::CommandBufferAllocateInfo(
			commandPool,
			vk::CommandBufferLevel::ePrimary,
			1
		)
	);
#pragma endregion

#pragma region VertexBuffer

	struct Vertex
	{
		float position[3];
		float color[3];
	};

	// Vertex buffer and attributes
	struct {
		vk::DeviceMemory memory;														// Handle to the device memory for this buffer
		vk::Buffer buffer;																// Handle to the Vulkan buffer object that the memory is bound to
		vk::PipelineVertexInputStateCreateInfo inputState;
		vk::VertexInputBindingDescription inputBinding;
		std::vector<vk::VertexInputAttributeDescription> inputAttributes;
	} vertices;

	// Setup vertices data
	std::vector<Vertex> vertexBuffer =
	{
		{ { 1.0f,  1.0f, 0.0f },{ 1.0f, 0.0f, 0.0f } },
		{ { -1.0f,  1.0f, 0.0f },{ 0.0f, 1.0f, 0.0f } },
		{ { 0.0f, -1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } }
	};

	uint32_t vertexBufferSize = static_cast<uint32_t>(vertexBuffer.size()) * sizeof(Vertex);

	void *data;

	struct StagingBuffer {
		vk::DeviceMemory memory;
		vk::Buffer buffer;
	};

	struct {
		StagingBuffer vertices;
		StagingBuffer indices;
	} stagingBuffers;

	// Vertex buffer
	stagingBuffers.vertices.buffer = logicalDevice.createBuffer(
		vk::BufferCreateInfo(
			vk::BufferCreateFlags(),
			vertexBufferSize,
			vk::BufferUsageFlagBits::eTransferSrc,
			vk::SharingMode::eExclusive,
			0,
			nullptr
		)
	);

	auto memReqs = logicalDevice.getBufferMemoryRequirements(stagingBuffers.vertices.buffer);

	// Request a host visible memory type that can be used to copy our data do
	// Also request it to be coherent, so that writes are visible to the GPU right after unmapping the buffer
	stagingBuffers.vertices.memory = logicalDevice.allocateMemory(
		vk::MemoryAllocateInfo(
			memReqs.size,
			getMemoryTypeIndex(memReqs.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent)
		)
	);

	// Map and copy
	data = logicalDevice.mapMemory(stagingBuffers.vertices.memory, 0, memReqs.size, vk::MemoryMapFlags());
	memcpy(data, vertexBuffer.data(), vertexBufferSize);
	logicalDevice.unmapMemory(stagingBuffers.vertices.memory);
	logicalDevice.bindBufferMemory(stagingBuffers.vertices.buffer, stagingBuffers.vertices.memory, 0);

	// Create a device local buffer to which the (host local) vertex data will be copied and which will be used for rendering
	vertices.buffer = logicalDevice.createBuffer(
		vk::BufferCreateInfo(
			vk::BufferCreateFlags(),
			vertexBufferSize,
			vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
			vk::SharingMode::eExclusive,
			0,
			nullptr
		)
	);

	memReqs = logicalDevice.getBufferMemoryRequirements(vertices.buffer);

	vertices.memory = logicalDevice.allocateMemory(
		vk::MemoryAllocateInfo(
			memReqs.size,
			getMemoryTypeIndex(memReqs.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal)
		)
	);

	logicalDevice.bindBufferMemory(vertices.buffer, vertices.memory, 0);

	// Vertex input binding
	vertices.inputBinding.binding = 0;
	vertices.inputBinding.stride = sizeof(Vertex);
	vertices.inputBinding.inputRate = vk::VertexInputRate::eVertex;

	// Inpute attribute binding describe shader attribute locations and memory layouts
	// These match the following shader layout (see triangle.vert):
	//	layout (location = 0) in vec3 inPos;
	//	layout (location = 1) in vec3 inColor;

	vertices.inputAttributes = {
		{0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, position)},
		{0, 1, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, color)}
	};

	// Assign to the vertex input state used for pipeline creation
	vertices.inputState = vk::PipelineVertexInputStateCreateInfo(
		vk::PipelineVertexInputStateCreateFlags(),
		1,
		&vertices.inputBinding,
		vertices.inputAttributes.size(),
		vertices.inputAttributes.data()
	);
#pragma endregion

#pragma region LoadShaders
	static auto readFile = [&](const std::string& filename) {
		std::ifstream file(filename, std::ios::ate | std::ios::binary);
		bool exists = (bool)file;

		if (!exists || !file.is_open()) {
			throw std::runtime_error("failed to open file!");
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	};

	auto vertShaderCode = readFile("C:/Users/Joshua Figuereo/Documents/Visual Studio 2017/Projects/raw-vulkan-examples/assets/bitmap.vert.spv");
	auto fragShaderCode = readFile("C:/Users/Joshua Figuereo/Documents/Visual Studio 2017/Projects/raw-vulkan-examples/assets/bitmap.frag.spv");
#pragma endregion

#pragma region Pipeline

	auto pipelineLayout = logicalDevice.createPipelineLayout(
		vk::PipelineLayoutCreateInfo(
			vk::PipelineLayoutCreateFlags(),
			0,
			nullptr,
			0,
			nullptr
		)
	);

	auto vertModule = logicalDevice.createShaderModule(
		vk::ShaderModuleCreateInfo(
			vk::ShaderModuleCreateFlags(),
			vertShaderCode.size(),
			(uint32_t*)vertShaderCode.data()
		)
	);

	auto fragModule = logicalDevice.createShaderModule(
		vk::ShaderModuleCreateInfo(
			vk::ShaderModuleCreateFlags(),
			fragShaderCode.size(),
			(uint32_t*)fragShaderCode.data()
		)
	);

	auto pipelineCache = logicalDevice.createPipelineCache(vk::PipelineCacheCreateInfo());

	std::vector<vk::PipelineShaderStageCreateInfo> pipelineShaderStages = {
		vk::PipelineShaderStageCreateInfo(
			vk::PipelineShaderStageCreateFlags(),
			vk::ShaderStageFlagBits::eVertex,
			vertModule,
			"main",
			nullptr
		),
		vk::PipelineShaderStageCreateInfo(
			vk::PipelineShaderStageCreateFlags(),
			vk::ShaderStageFlagBits::eFragment,
			fragModule,
			"main",
			nullptr
		)
	};

	auto pvi = vertices.inputState;

	auto pia = vk::PipelineInputAssemblyStateCreateInfo(
		vk::PipelineInputAssemblyStateCreateFlags(),
		vk::PrimitiveTopology::eTriangleList
	);

	auto pt = vk::PipelineTessellationStateCreateInfo(
	);

	auto pv = vk::PipelineViewportStateCreateInfo(
		vk::PipelineViewportStateCreateFlagBits(),
		1,
		&viewport,
		1,
		&renderSize
	);

	auto pr = vk::PipelineRasterizationStateCreateInfo(
		vk::PipelineRasterizationStateCreateFlags(),
		VK_FALSE,
		VK_FALSE,
		vk::PolygonMode::eFill,
		vk::CullModeFlagBits::eNone,
		vk::FrontFace::eCounterClockwise,
		VK_FALSE,
		0,
		0,
		0,
		1.0f
	);

	auto pm = vk::PipelineMultisampleStateCreateInfo(
		vk::PipelineMultisampleStateCreateFlags(),
		vk::SampleCountFlagBits::e1
	);

	// Dept and Stencil state for primative compare/test operations

	auto pds = vk::PipelineDepthStencilStateCreateInfo(
		vk::PipelineDepthStencilStateCreateFlags(),
		VK_TRUE,
		VK_TRUE,
		vk::CompareOp::eLessOrEqual,
		VK_FALSE,
		VK_FALSE,
		vk::StencilOpState(),
		vk::StencilOpState(),
		0,
		0
	);

	// Blend State - How two primatives should draw on top of each other.
	std::vector<vk::PipelineColorBlendAttachmentState> colorBlendAttachments =
	{
		vk::PipelineColorBlendAttachmentState(
			VK_FALSE,
			vk::BlendFactor::eZero,
			vk::BlendFactor::eOne,
			vk::BlendOp::eAdd,
			vk::BlendFactor::eZero,
			vk::BlendFactor::eZero,
			vk::BlendOp::eAdd,
			vk::ColorComponentFlags(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
		)
	};

	auto pbs = vk::PipelineColorBlendStateCreateInfo(
		vk::PipelineColorBlendStateCreateFlags(),
		0,
		vk::LogicOp::eClear,
		colorBlendAttachments.size(),
		colorBlendAttachments.data()
	);

	std::vector<vk::DynamicState> dynamicStates =
	{
		vk::DynamicState::eViewport,
		vk::DynamicState::eScissor
	};

	auto pdy = vk::PipelineDynamicStateCreateInfo(
		vk::PipelineDynamicStateCreateFlags(),
		dynamicStates.size(),
		dynamicStates.data()
	);

	auto graphicsPipeline = logicalDevice.createGraphicsPipeline(pipelineCache,
		vk::GraphicsPipelineCreateInfo(
			vk::PipelineCreateFlags(vk::PipelineCreateFlagBits::eDerivative),
			pipelineShaderStages.size(),
			pipelineShaderStages.data(),
			&pvi,
			&pia,
			nullptr,
			&pv,
			&pr,
			&pm,
			&pds,
			&pbs,
			&pdy,
			pipelineLayout,
			renderpass,
			0
		)
	);

#pragma endregion

#pragma region Commands

	std::vector<vk::ClearValue> clearValues =
	{
		vk::ClearColorValue(
			std::array<float,4>{0.5f, 0.5f, 0.5f, 1.0f}
	  )
	};

	// From here we can do common GL commands
	// Lets add commands to each command buffer.
	commandBuffers[0].begin(vk::CommandBufferBeginInfo());
	commandBuffers[0].beginRenderPass(
		vk::RenderPassBeginInfo(
			renderpass,
			outputFrameBuffer,
			renderSize,
			clearValues.size(),
			clearValues.data()
		),
		vk::SubpassContents::eInline
	);

	commandBuffers[0].setViewport(0, 1, &viewport);
	commandBuffers[0].setScissor(0, 1, &renderSize);
	commandBuffers[0].bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline);

	std::array<vk::DeviceSize, 1> offsets = { 0 };
	commandBuffers[0].bindVertexBuffers(0, 1, &vertices.buffer, offsets.data());
	commandBuffers[0].draw(3, 1, 0, 0);
	commandBuffers[0].endRenderPass();
	commandBuffers[0].end();

#pragma endregion

#pragma region SaveToFile
	auto outputImageSize = logicalDevice.getImageMemoryRequirements(outputImage);

	auto outputBuffer = logicalDevice.createBuffer(
		vk::BufferCreateInfo(
			vk::BufferCreateFlags(),
			outputImageSize.size,
			vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eStorageBuffer,
			vk::SharingMode::eExclusive,
			0,
			nullptr
		)
	);

	auto outputBufferMemory = logicalDevice.allocateMemory(
		vk::MemoryAllocateInfo(
			outputImageSize.size,
			getMemoryTypeIndex(memReqs.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent)
		)
	);

	logicalDevice.bindBufferMemory(outputBuffer, outputBufferMemory, 0);

	std::vector<vk::BufferImageCopy> copyRegions =
	{
		vk::BufferImageCopy(
			outputImageSize.alignment,
			renderSize.extent.width,
			renderSize.extent.height,
			vk::ImageSubresourceLayers(),
			vk::Offset3D(),
			vk::Extent3D(renderSize.extent.width, renderSize.extent.height, 1)
)
	};

	// Copy output buffer to host local memory.
	logicalDevice.waitIdle();
	commandBuffers[0].reset(vk::CommandBufferResetFlagBits::eReleaseResources);
	commandBuffers[0].copyImageToBuffer(outputImage, vk::ImageLayout(), outputBuffer, copyRegions);

	// Wait for every task on the GPU to finish
	logicalDevice.waitIdle();

	data = logicalDevice.mapMemory(outputMemory, 0, memReqs.size, vk::MemoryMapFlags());

	// Interate on every byte of the image data and write it to a file.

	typedef unsigned char byte;

	typedef struct
	{
		byte red, green, blue;
	}
	RGB_t;

	// It is presumed that the image is stored in memory as 
	//   RGB_t data[ height ][ width ]
	// where lines are top to bottom and columns are left to right
	// (the same way you view the image on the display)

	// The routine makes all the appropriate adjustments to match the TGA format specification.

	auto write_truecolor_tga = [](const std::string& filename, RGB_t* data, uint32_t width, uint32_t height)
	{
		std::ofstream tgafile(filename.c_str(), std::ios::binary);
		if (!tgafile) return false;

		// The image header
		byte header[18] = { 0 };
		header[2] = 1;  // truecolor
		header[12] = width & 0xFF;
		header[13] = (width >> 8) & 0xFF;
		header[14] = height & 0xFF;
		header[15] = (height >> 8) & 0xFF;
		header[16] = 24;  // bits per pixel

		tgafile.write((const char*)header, 18);

		// The image data is stored bottom-to-top, left-to-right
		for (int y = height - 1; y >= 0; y--)
			for (int x = 0; x < width; x++)
			{
				tgafile.put((char)data[(y * width) + x].blue);
				tgafile.put((char)data[(y * width) + x].green);
				tgafile.put((char)data[(y * width) + x].red);
			}

		// The file footer. This part is totally optional.
		static const char footer[26] =
			"\0\0\0\0"  // no extension area
			"\0\0\0\0"  // no developer directory
			"TRUEVISION-XFILE"  // yep, this is a TGA file
			".";
		tgafile.write(footer, 26);

		tgafile.close();
		return true;
	};

	write_truecolor_tga("output.tga", (RGB_t*)data, surfaceSize.width, surfaceSize.height);
#pragma endregion

	return 0;
}