#pragma once

#include "../Core/Renderer.h"

namespace raw
{
class RTX : public Renderer
{
  protected:
    /**
     * Bottom Level Acceleation Structure
     */
    struct AccelerationStructureHandle
    {
        vk::AccelerationStructureNV accel;
        vk::Memory memory;
    };

    struct RasterBuffer
    {
        vk::Buffer buffer;
        vk::Memory memory;
    };

    struct CornellMesh
    {
    AccelerationStructureHandle BLAS;
    AccelerationStructureHandle TLAS;

    RasterBuffer indices;
    RasterBuffer vertices;
    RasterBuffer texCoords;
    RasterBuffer colors;

    }

    struct PathTracingMaterial
    {
        vk::Pipeline rayPipeline;
        vk::Buffer shaderBindingTableBuffer;
        vk::ShaderModule rayGen;
        vk::ShaderModule closestHit;
        vk::ShaderModule miss;
    };

    PathTracingMaterial material;



    struct Skybox
    {
        vk::Image image;
        vk::ImageView view;
    };

    Skybox skybox;
};
};
}