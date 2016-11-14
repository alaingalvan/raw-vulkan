#ifdef _MSC_VER
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif
#define VK_USE_PLATFORM_WIN32_KHR
#define __INT32_TYPE__
#define USE_SWAPCHAIN_EXTENSIONS
#include "windows.h"

#include "stdio.h"
#include <chrono>
#include <exception>
#include <algorithm>

#include "vulkan/vulkan.hpp"
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"

int main(const int argc, const char *argv[])
{

#pragma region StartInstance

	auto installedExtensions = vk::enumerateInstanceExtensionProperties();

	std::vector<const char*> wantedExtensions =
	{
		VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
		VK_KHR_SURFACE_EXTENSION_NAME,
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME
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
		"MyApp",
		VK_MAKE_VERSION(1, 0, 0),
		"MyAppEngine",
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
	auto gpu = physicalDevices[0];
	auto gpuProps = gpu.getProperties();
	auto gpuMemoryProps = gpu.getMemoryProperties();

#pragma endregion

#pragma region LogicalDevice
	auto gpuExtensions = gpu.enumerateDeviceExtensionProperties();

	// Init Device Extension/Validation layers
	std::vector<const char*> wantedDeviceExtensions =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_EXT_DEBUG_MARKER_EXTENSION_NAME
	};

	auto deviceExtensions = std::vector<const char*>();

	for (auto &w : wantedDeviceExtensions) {
		for (auto &i : gpuExtensions) {
			if (std::string(i.extensionName).compare(w) == 0) {
				deviceExtensions.emplace_back(w);
				break;
			}
		}
	}

	auto gpuLayers = gpu.enumerateDeviceLayerProperties();

	std::vector<const char*> wantedDeviceValidationLayers =
	{
		"VK_LAYER_LUNARG_standard_validation",
		"VK_LAYER_RENDERDOC_Capture"
	};

	auto deviceValidationLayers = std::vector<const char*>();

	for (auto &w : wantedLayers) {
		for (auto &i : installedLayers) {
			if (std::string(i.layerName).compare(w) == 0) {
				layers.emplace_back(w);
				break;
			}
		}
	}



	auto gpuFeatures = gpu.getFeatures();
	auto gpuQueueProps = gpu.getQueueFamilyProperties();

	float priority = 0.0;
	uint32_t graphicsFamilyIndex = 0;
	auto queueCreateInfos = std::vector<vk::DeviceQueueCreateInfo>();

	for (auto& queuefamily : gpuQueueProps)
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

	auto device = gpu.createDevice(
		vk::DeviceCreateInfo(
			vk::DeviceCreateFlags(),
			queueCreateInfos.size(),
			queueCreateInfos.data(),
			deviceValidationLayers.size(),
			deviceValidationLayers.data(),
			deviceExtensions.size(),
			deviceExtensions.data(),
			&gpuFeatures
		)
	);

#pragma endregion

#pragma region Queue
	auto graphicsQueue = device.getQueue(graphicsFamilyIndex, 0);
#pragma endregion

#pragma region Window
	std::string title = "MyVulkanApp";
	std::string name = "MyVulkanApp";
	uint32_t width = 1280;
	uint32_t height = 720;
	auto hInstance = GetModuleHandle(0);

	WNDCLASSEX wndClass;
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = [](HWND h, UINT m, WPARAM w, LPARAM l)->LRESULT
	{
		if (m == WM_CLOSE)
			PostQuitMessage(0);
		else
			return DefWindowProc(h, m, w, l);
		return 0;
	};
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = name.c_str();
	wndClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

	if (!RegisterClassEx(&wndClass)) {
		fflush(stdout);
		exit(1);
	}

	DWORD dwExStyle;
	DWORD dwStyle;

	dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	RECT windowRect;
	windowRect.left = 0L;
	windowRect.top = 0L;
	windowRect.right = (long)width;
	windowRect.bottom = (long)height;

	AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

	auto window = CreateWindowEx(0,
		name.c_str(),
		title.c_str(),
		dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		0,
		0,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		hInstance,
		NULL);

	// Center on screen
	uint32_t x = (GetSystemMetrics(SM_CXSCREEN) - windowRect.right) / 2;
	uint32_t y = (GetSystemMetrics(SM_CYSCREEN) - windowRect.bottom) / 2;
	SetWindowPos(window, 0, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

	if (!window) {
		printf("Could not create window!\n");
		fflush(stdout);
		exit(1);
	}

	ShowWindow(window, SW_SHOW);
	SetForegroundWindow(window);
	SetFocus(window);
#pragma endregion

#pragma region Surface

	// Screen Size
	auto surfaceSize = vk::Extent2D(width, height);
	auto renderArea = vk::Rect2D(vk::Offset2D(), surfaceSize);
	auto viewport = vk::Viewport(0.0f, 0.0f, width, height, 0, 1.0f);

	std::vector<vk::Viewport> viewports =
	{
		viewport
	};

	std::vector<vk::Rect2D> scissors =
	{
		renderArea
	};

	auto surfaceInfo = vk::Win32SurfaceCreateInfoKHR(vk::Win32SurfaceCreateFlagsKHR(), hInstance, window);
	auto vkSurfaceInfo = surfaceInfo.operator const VkWin32SurfaceCreateInfoKHR&();

	auto vksurface = VkSurfaceKHR();
	auto createwin32surface = vkCreateWin32SurfaceKHR(instance, &vkSurfaceInfo, NULL, &vksurface);
	assert(createwin32surface == VK_SUCCESS);

	// Get surface information
	auto surface = vk::SurfaceKHR(vksurface);

#pragma endregion

#pragma region ColorFormats
	// Check to see if we can display rgb colors.
	auto surfaceFormats = gpu.getSurfaceFormatsKHR(surface);

	vk::Format surfaceColorFormat;
	vk::ColorSpaceKHR surfaceColorSpace;

	if (surfaceFormats.size() == 1 && surfaceFormats[0].format == vk::Format::eUndefined)
		surfaceColorFormat = vk::Format::eB8G8R8A8Unorm;
	else
		surfaceColorFormat = surfaceFormats[0].format;

	surfaceColorSpace = surfaceFormats[0].colorSpace;


	auto formatProperties = gpu.getFormatProperties(vk::Format::eR8G8B8A8Unorm);

	// Since all depth formats may be optional, we need to find a suitable depth format to use
	// Start with the highest precision packed format
	std::vector<vk::Format> depthFormats = {
		vk::Format::eD32SfloatS8Uint,
		vk::Format::eD32Sfloat,
		vk::Format::eD24UnormS8Uint,
		vk::Format::eD16UnormS8Uint,
		vk::Format::eD16Unorm
	};

	vk::Format surfaceDepthFormat;

	for (auto& format : depthFormats)
	{
		auto depthFormatProperties = gpu.getFormatProperties(format);
		// Format must support depth stencil attachment for optimal tiling
		if (depthFormatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
		{
			surfaceDepthFormat = format;
			break;
		}
	}
#pragma endregion

#pragma region RenderPass
	std::vector<vk::AttachmentDescription> attachmentDescriptions =
	{
		vk::AttachmentDescription(
			vk::AttachmentDescriptionFlags(),
			surfaceColorFormat,
			vk::SampleCountFlagBits::e1,
			vk::AttachmentLoadOp::eClear,
			vk::AttachmentStoreOp::eStore,
			vk::AttachmentLoadOp::eDontCare,
			vk::AttachmentStoreOp::eDontCare,
			vk::ImageLayout::eUndefined,
			vk::ImageLayout::ePresentSrcKHR
		),
		vk::AttachmentDescription(
			vk::AttachmentDescriptionFlags(),
			surfaceDepthFormat,
			vk::SampleCountFlagBits::e1,
			vk::AttachmentLoadOp::eClear,
			vk::AttachmentStoreOp::eDontCare,
			vk::AttachmentLoadOp::eDontCare,
			vk::AttachmentStoreOp::eDontCare,
			vk::ImageLayout::eUndefined,
			vk::ImageLayout::eDepthStencilAttachmentOptimal
		)
	};

	std::vector<vk::AttachmentReference> colorReferences =
	{
		vk::AttachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal)
	};

	std::vector<vk::AttachmentReference> depthReferences = {
		vk::AttachmentReference(1, vk::ImageLayout::eDepthStencilAttachmentOptimal)
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
			depthReferences.data(),
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
		),
		vk::SubpassDependency(
			0,
			~0U,
			vk::PipelineStageFlagBits::eColorAttachmentOutput,
			vk::PipelineStageFlagBits::eBottomOfPipe,
			vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite,
			vk::AccessFlagBits::eMemoryRead,
			vk::DependencyFlagBits::eByRegion
		)
	};

	auto renderpass = device.createRenderPass(
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

#pragma region Swapchain

	auto surfaceCapabilities = gpu.getSurfaceCapabilitiesKHR(surface);
	auto surfacePresentModes = gpu.getSurfacePresentModesKHR(surface);

	// check the surface width/height.
	if (!(surfaceCapabilities.currentExtent.width == -1 || surfaceCapabilities.currentExtent.height == -1)) {
		surfaceSize = surfaceCapabilities.currentExtent;
	}

	auto presentMode = vk::PresentModeKHR::eImmediate;

	for (auto& pm : surfacePresentModes) {
		if (pm == vk::PresentModeKHR::eMailbox) {
			presentMode = vk::PresentModeKHR::eMailbox;
			break;
		}
	}

	assert(surfaceCapabilities.maxImageCount >= 3);


	std::vector<uint32_t> queueFamilyIndices;
	queueFamilyIndices.push_back(graphicsFamilyIndex);

	auto swapchain = device.createSwapchainKHR(
		vk::SwapchainCreateInfoKHR(
			vk::SwapchainCreateFlagsKHR(),
			surface,
			surfaceCapabilities.maxImageCount,
			surfaceColorFormat,
			surfaceColorSpace,
			surfaceSize,
			1,
			vk::ImageUsageFlagBits::eColorAttachment,
			vk::SharingMode::eExclusive,
			queueFamilyIndices.size(),
			queueFamilyIndices.data(),
			vk::SurfaceTransformFlagBitsKHR::eIdentity,
			vk::CompositeAlphaFlagBitsKHR::eOpaque,
			presentMode,
			VK_TRUE,
			vk::SwapchainKHR()
		)
	);

#pragma endregion

#pragma region FrameBuffers
	// The swapchain handles allocating frame images.
	auto swapchainImages = device.getSwapchainImagesKHR(swapchain);

	// Create Depth Image Data
	auto depthImage = device.createImage(
		vk::ImageCreateInfo(
			vk::ImageCreateFlags(),
			vk::ImageType::e2D,
			surfaceDepthFormat,
			vk::Extent3D(surfaceSize.width, surfaceSize.height, 1),
			1,
			1,
			vk::SampleCountFlagBits::e1,
			vk::ImageTiling::eOptimal,
			vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eTransferSrc,
			vk::SharingMode::eExclusive,
			queueFamilyIndices.size(),
			queueFamilyIndices.data(),
			vk::ImageLayout::eUndefined
		)
	);

	// Search through GPU memory properies to see if this can be device local.

	auto depthMemoryReq = device.getImageMemoryRequirements(depthImage);
	uint32_t typeBits = depthMemoryReq.memoryTypeBits;
	uint32_t depthMemoryTypeIndex;

	for (uint32_t i = 0; i < gpuMemoryProps.memoryTypeCount; i++)
	{
		if ((typeBits & 1) == 1)
		{
			if ((gpuMemoryProps.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlagBits::eDeviceLocal) == vk::MemoryPropertyFlagBits::eDeviceLocal)
			{
				depthMemoryTypeIndex = i;
				break;
			}
		}
		typeBits >>= 1;
	}

	auto depthMemory = device.allocateMemory(
		vk::MemoryAllocateInfo(depthMemoryReq.size, depthMemoryTypeIndex)
	);


	device.bindImageMemory(
		depthImage,
		depthMemory,
		0
	);

	auto depthImageView = device.createImageView(
		vk::ImageViewCreateInfo(
			vk::ImageViewCreateFlags(),
			depthImage,
			vk::ImageViewType::e2D,
			surfaceDepthFormat,
			vk::ComponentMapping(),
			vk::ImageSubresourceRange(
				vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil,
				0,
				1,
				0,
				1
			)
		)
	);

	struct SwapChainBuffer {
		vk::Image image;
		std::array<vk::ImageView, 2> views;
		vk::Framebuffer frameBuffer;
	};

	std::vector<SwapChainBuffer> swapchainBuffers;
	swapchainBuffers.resize(swapchainImages.size());

	for (int i = 0; i < swapchainImages.size(); i++)
	{
		swapchainBuffers[i].image = swapchainImages[i];

		// Color
		swapchainBuffers[i].views[0] =
			device.createImageView(
				vk::ImageViewCreateInfo(
					vk::ImageViewCreateFlags(),
					swapchainImages[i],
					vk::ImageViewType::e1D,
					surfaceColorFormat,
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

		// Depth
		swapchainBuffers[i].views[1] = depthImageView;

		swapchainBuffers[i].frameBuffer = device.createFramebuffer(
			vk::FramebufferCreateInfo(
				vk::FramebufferCreateFlags(),
				renderpass,
				swapchainBuffers[i].views.size(),
				swapchainBuffers[i].views.data(),
				surfaceSize.width,
				surfaceSize.height,
				1
			)
		);
	}
#pragma endregion

#pragma region Synchronization

	// Semaphore used to ensures that image presentation is complete before starting to submit again
	auto presentCompleteSemaphore = device.createSemaphore(vk::SemaphoreCreateInfo());

	// Semaphore used to ensures that all commands submitted have been finished before submitting the image to the queue
	auto renderCompleteSemaphore = device.createSemaphore(vk::SemaphoreCreateInfo());

	// Fence for command buffer completion
	std::vector<vk::Fence> waitFences;
	waitFences.resize(swapchainBuffers.size());
	for (int i = 0; i < waitFences.size(); i++)
	{
		waitFences[i] = device.createFence(vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
	}

#pragma endregion

#pragma region CommandPool

	auto commandPool = device.createCommandPool(
		vk::CommandPoolCreateInfo(
			vk::CommandPoolCreateFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer),
			graphicsFamilyIndex
		)
	);

	// Allocate one buffer for each frame in the Swapchain.
	auto commandBuffers = device.allocateCommandBuffers(
		vk::CommandBufferAllocateInfo(
			commandPool,
			vk::CommandBufferLevel::ePrimary,
			swapchainBuffers.size()
		)
	);
#pragma endregion

#pragma region DescriptorPool
	std::vector<vk::DescriptorPoolSize> descriptorPoolSizes =
	{
		vk::DescriptorPoolSize(
		vk::DescriptorType::eUniformBuffer,
		1
		)
	};

	auto descriptorPool = device.createDescriptorPool(
		vk::DescriptorPoolCreateInfo(
			vk::DescriptorPoolCreateFlags(),
			1,
			descriptorPoolSizes.size(),
			descriptorPoolSizes.data()
		)
	);
#pragma endregion

#pragma region DescriptorSetLayout
	// Binding 0: Uniform buffer (Vertex shader)
	std::vector<vk::DescriptorSetLayoutBinding> descriptorSetLayoutBindings =
	{
		vk::DescriptorSetLayoutBinding(
			0,
			vk::DescriptorType::eUniformBuffer,
			1,
			vk::ShaderStageFlagBits::eVertex,
			nullptr
		)
	};

	std::vector<vk::DescriptorSetLayout> descriptorSetLayouts = {
		device.createDescriptorSetLayout(
			vk::DescriptorSetLayoutCreateInfo(
				vk::DescriptorSetLayoutCreateFlags(),
				descriptorSetLayoutBindings.size(),
				descriptorSetLayoutBindings.data()
		)
		)
	};

	auto descriptorSets = device.allocateDescriptorSets(
		vk::DescriptorSetAllocateInfo(
			descriptorPool,
			descriptorSetLayouts.size(),
			descriptorSetLayouts.data()
		)
	);

#pragma endregion
	/*
#pragma region Pipeline
	std::vector<vk::PushConstantRange> pushConstants = {

	};

	auto pipelineLayout = device.createPipelineLayout(
		vk::PipelineLayoutCreateInfo(
			vk::PipelineLayoutCreateFlags(),
			descriptorSetLayouts.size(),
			descriptorSetLayouts.data(),
			pushConstants.size(),
			pushConstants.data()
		)
	);

	auto vertModule = device.createShaderModule(
		vk::ShaderModuleCreateInfo(
			vk::ShaderModuleCreateFlags(),
			0,
			nullptr
		)
	);

	auto fragModule = device.createShaderModule(
		vk::ShaderModuleCreateInfo(
			vk::ShaderModuleCreateFlags(),
			0,
			nullptr)
	);

	auto pipelineCache = device.createPipelineCache(vk::PipelineCacheCreateInfo());

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

	auto pvi = vk::PipelineVertexInputStateCreateInfo(
		vk::PipelineVertexInputStateCreateFlags(),
		0
	);

	auto pia = vk::PipelineInputAssemblyStateCreateInfo(
	);

	auto pt = vk::PipelineTessellationStateCreateInfo(
	);

	auto pv = vk::PipelineViewportStateCreateInfo(
		vk::PipelineViewportStateCreateFlagBits(),
		viewports.size(),
		viewports.data(),
		scissors.size(),
		scissors.data()
	);

	auto pr = vk::PipelineRasterizationStateCreateInfo(
	);

	auto pm = vk::PipelineMultisampleStateCreateInfo(
		vk::PipelineMultisampleStateCreateFlags(),
		vk::SampleCountFlagBits::e1
	);

	// Dept and Stencil state for primative compare/test operations

	auto pds = vk::PipelineDepthStencilStateCreateInfo(
		vk::PipelineDepthStencilStateCreateFlags(),
		VK_FALSE,
		VK_FALSE,
		vk::CompareOp::eLess,
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
			vk::BlendFactor::eOne,
			vk::BlendFactor::eOne,
			vk::BlendOp::eAdd,
			vk::BlendFactor::eOne,
			vk::BlendFactor::eOne,
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

	auto pdy = vk::PipelineDynamicStateCreateInfo(
	);

	auto graphicsPipeline = device.createGraphicsPipeline(pipelineCache,
		vk::GraphicsPipelineCreateInfo(
			vk::PipelineCreateFlags(vk::PipelineCreateFlagBits::eDerivative),
			pipelineShaderStages.size(),
			pipelineShaderStages.data(),
			&pvi,
			&pia,
			&pt,
			&pv,
			&pr,
			&pm,
			&pds,
			&pbs,
			&pdy,
			pipelineLayout,
			renderpass
		)
	);

#pragma endregion
	/*
#pragma region VertexBuffer

	struct Vertex
	{
		float position[3];
		float color[3];
	};

	// Vertex buffer and attributes
	struct {
		VkDeviceMemory memory;															// Handle to the device memory for this buffer
		VkBuffer buffer;																// Handle to the Vulkan buffer object that the memory is bound to
		VkPipelineVertexInputStateCreateInfo inputState;
		VkVertexInputBindingDescription inputBinding;
		std::vector<VkVertexInputAttributeDescription> inputAttributes;
	} vertices;

	// Index buffer
	struct
	{
		VkDeviceMemory memory;
		VkBuffer buffer;
		uint32_t count;
	} indices;

	// Uniform block object
	struct {
		VkDeviceMemory memory;
		VkBuffer buffer;
		VkDescriptorBufferInfo descriptor;
	}  uniformDataVS;

	// For simplicity we use the same uniform block layout as in the shader:
	//
	//	layout(set = 0, binding = 0) uniform UBO
	//	{
	//		mat4 projectionMatrix;
	//		mat4 modelMatrix;
	//		mat4 viewMatrix;
	//	} ubo;
	//
	// This way we can just memcopy the ubo data to the ubo
	// Note: You should use data types that align with the GPU in order to avoid manual padding (vec4, mat4)
	struct {
		glm::mat4 projectionMatrix;
		glm::mat4 modelMatrix;
		glm::mat4 viewMatrix;
	} uboVS;

	// Setup vertices
	std::vector<Vertex> vertexBuffer =
	{
		{ { 1.0f,  1.0f, 0.0f },{ 1.0f, 0.0f, 0.0f } },
		{ { -1.0f,  1.0f, 0.0f },{ 0.0f, 1.0f, 0.0f } },
		{ { 0.0f, -1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } }
	};
	uint32_t vertexBufferSize = static_cast<uint32_t>(vertexBuffer.size()) * sizeof(Vertex);

	// Setup indices
	std::vector<uint32_t> indexBuffer = { 0, 1, 2 };
	indices.count = static_cast<uint32_t>(indexBuffer.size());
	uint32_t indexBufferSize = indices.count * sizeof(uint32_t);

	device.createBuffer(
		vk::BufferCreateInfo()
	);
#pragma endregion

#pragma region Commands


	std::vector<vk::ClearValue> clearValues =
	{
		vk::ClearColorValue(
			std::array<float,4>{0.0f, 0.0f, 0.2f, 1.0f}
	  )
	};

	// From here we can do common GL commands
	// Lets add commands to each command buffer.
	for (int32_t i = 0; i < commandBuffers.size(); ++i)
	{
		commandBuffers[i].begin(vk::CommandBufferBeginInfo());
		commandBuffers[i].beginRenderPass(
			vk::RenderPassBeginInfo(
				renderpass,
				swapchainBuffers[i].frameBuffer,
				renderArea,
				clearValues.size(),
				clearValues.data()
			),
			vk::SubpassContents::eInline
		);

		commandBuffers[i].setViewport(0, viewports);

		commandBuffers[i].setScissor(0, scissors);

		// Bind Descriptor Sets, these are attribute/uniform "descriptions"
		commandBuffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline);

		commandBuffers[i].bindDescriptorSets(
			vk::PipelineBindPoint::eGraphics,
			pipelineLayout,
			0,
			descriptorSets,
			nullptr
		);

		//commandBuffers[i].bindVertexBuffers(0, vertexBuffers, offsets);
		commandBuffers[i].bindIndexBuffer(indices.buffer, 0, vk::IndexType::eUint32);
		commandBuffers[i].drawIndexed(indices.count, 1, 0, 0, 1);
		commandBuffers[i].endRenderPass();
		//commandBuffers[i].pipelineBarrier();
		commandBuffers[i].end();
	}
#pragma endregion

#pragma region SubmitCommandBuffers
	// Create kernels to submit to the queue on a given render pass.
	auto kernelPipelineStageFlags = vk::PipelineStageFlags::Flags(vk::PipelineStageFlagBits::eAllCommands);
	auto kernel = vk::SubmitInfo(
		0U,
		nullptr,
		&kernelPipelineStageFlags,
		commandBuffers.size(),
		commandBuffers.data(),
		0U,
		nullptr
	);
	std::vector<vk::SubmitInfo> kernels = {
		kernel
	};

	uint32_t imageIndex = 0;

	graphicsQueue.submit(kernels, NULL);
	graphicsQueue.presentKHR(
		vk::PresentInfoKHR(
			0,
			nullptr,
			1,
			&swapchain,
			&imageIndex,
			nullptr
		)
	);

	//	device.acquireNextImageKHR(swapchain, UINT64_MAX, nullptr, nullptr);
#pragma endregion
*/
#pragma region RenderLoop
	MSG msg;
	uint64_t frameCounter = 0;
	double frameTimer = 0.0;
	double fpsTimer = 0.0;
	double lastFPS = 0.0;

	while (TRUE)
	{
		auto tStart = std::chrono::high_resolution_clock::now();

		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
		{
			break;
		}

		//render();

		frameCounter++;
		auto tEnd = std::chrono::high_resolution_clock::now();
		auto tDiff = std::chrono::duration<double, std::milli>(tEnd - tStart).count();
		frameTimer = tDiff / 1000.0;

		fpsTimer += tDiff;
		if (fpsTimer > 1000.0)
		{
			std::string windowTitle = title + " - " + std::to_string(frameCounter) + " fps";
			SetWindowText(window, windowTitle.c_str());

			lastFPS = roundf(1.0 / frameTimer);
			fpsTimer = 0.0;
			frameCounter = 0;
		}
	}
#pragma endregion

	device.waitIdle();

	return 0;
}