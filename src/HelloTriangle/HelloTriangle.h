#pragma once

#include "../Core/Renderer.h"

namespace raw
{
class HelloTriangle : public Renderer
{
  public:
    virtual void initializeResources() override;

    // Destroy any resources used in this example
    virtual void destroyResources() override;

    // Create graphics API specific data structures to send commands to the GPU
    virtual void createCommands() override;

    // Set up commands used when rendering frame by this app
    virtual void setupCommands() override;

  protected:
    struct Vertex
    {
        float position[3];
        float color[3];
    };

    Vertex mVertexBufferData[3] = {{{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
                                   {{-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
                                   {{0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}}};

    uint32_t mIndexBufferData[3] = {0, 1, 2};

    vk::DescriptorPool mDescriptorPool;
    std::vector<vk::DescriptorSetLayout> mDescriptorSetLayouts;
    std::vector<vk::DescriptorSet> mDescriptorSets;

    vk::ShaderModule mVertModule;
    vk::ShaderModule mFragModule;

    vk::RenderPass mRenderPass;

    vk::Buffer mVertexBuffer;
    vk::Buffer mIndexBuffer;

    vk::PipelineCache mPipelineCache;
    vk::Pipeline mPipeline;
    vk::PipelineLayout mPipelineLayout;

    // Vertex buffer and attributes
    struct
    {
        vk::DeviceMemory memory; // Handle to the device memory for this buffer
        vk::Buffer buffer; // Handle to the Vulkan buffer object that the memory
                           // is bound to
        vk::PipelineVertexInputStateCreateInfo inputState;
        vk::VertexInputBindingDescription inputBinding;
        std::vector<vk::VertexInputAttributeDescription> inputAttributes;
    } mVertices;

    // Index buffer
    struct
    {
        vk::DeviceMemory memory;
        vk::Buffer buffer;
        uint32_t count;
    } mIndices;

    // Uniform block object
    struct
    {
        vk::DeviceMemory memory;
        vk::Buffer buffer;
        vk::DescriptorBufferInfo descriptor;
    } mUniformDataVS;

    // Uniform data
    struct
    {
        Matrix4 projectionMatrix;
        Matrix4 modelMatrix;
        Matrix4 viewMatrix;
    } uboVS;
};
}