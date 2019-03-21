#pragma once

#include "CrossWindow/CrossWindow.h"
#include "CrossWindow/Graphics.h"
#include "vectormath.hpp"

#include <vector>
#include <chrono>

namespace raw
{
// Renderer

class Renderer
{
public:
	Renderer(xwin::Window& window);

	~Renderer();

	// Render onto the render target
	virtual void render();

	// Resize the window and internal data structures
	void resize(unsigned width, unsigned height);

protected:

	// Initialize your Graphics API
	virtual void initializeAPI(xwin::Window& window);

	// Destroy any Graphics API data structures used in this example
	virtual void destroyAPI();

	// Initialize any resources such as VBOs, IBOs, used in this example
	virtual void initializeResources();

	// Destroy any resources used in this example
	virtual void destroyResources();

	// Create graphics API specific data structures to send commands to the GPU
	virtual void createCommands();

	// Set up commands used when rendering frame by this app
	virtual void setupCommands();

	// Destroy all commands
	virtual void destroyCommands();

	// Set up the FrameBuffer
	virtual void initFrameBuffer();

	virtual void destroyFrameBuffer();

	// Set up the RenderPass
	virtual void createRenderPass();

	virtual void createSynchronization();

	// Set up the swapchain
	virtual void setupSwapchain(unsigned width, unsigned height);

	std::chrono::time_point<std::chrono::steady_clock> tStart, tEnd;
	float mElapsedTime = 0.0f;

	// Uniform data
	struct {
		Matrix4 projectionMatrix;
		Matrix4 modelMatrix;
		Matrix4 viewMatrix;
	} uboVS;

	// Initialization
	vk::Instance mInstance;
	vk::PhysicalDevice mPhysicalDevice;
	vk::Device mDevice;

	vk::SwapchainKHR mSwapchain;
	vk::SurfaceKHR mSurface;

	float mQueuePriority;
	vk::Queue mQueue;
	uint32_t mQueueFamilyIndex;

	vk::CommandPool mCommandPool;
	std::vector<vk::CommandBuffer> mCommandBuffers;
	uint32_t mCurrentBuffer;

	vk::Extent2D mSurfaceSize;
	vk::Rect2D mRenderArea;
	vk::Viewport mViewport;

	// Resources
	vk::Format mSurfaceColorFormat;
	vk::ColorSpaceKHR mSurfaceColorSpace;
	vk::Format mSurfaceDepthFormat;
	vk::Image mDepthImage;
	vk::DeviceMemory mDepthImageMemory;

	vk::DescriptorPool mDescriptorPool;
	std::vector<vk::DescriptorSetLayout> mDescriptorSetLayouts;
	std::vector<vk::DescriptorSet> mDescriptorSets;

	// Sync
	vk::Semaphore mPresentCompleteSemaphore;
	vk::Semaphore mRenderCompleteSemaphore;
	std::vector<vk::Fence> mWaitFences;

	// Swpachain
	struct SwapChainBuffer {
		vk::Image image;
		std::array<vk::ImageView, 2> views;
		vk::Framebuffer frameBuffer;
	};

	std::vector<SwapChainBuffer> mSwapchainBuffers;

};
}