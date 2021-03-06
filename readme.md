# Raw Vulkan

[![cmake-img]][cmake-url]
[![License][license-img]][license-url]

An example seed project introducing programming a C++ Vulkan application.

## Getting Started

First install [Git](https://git-scm.com/downloads), then open any terminal such as [Hyper](https://hyper.is/) in any folder and type:

```bash
# 🐑 Clone the repo
git clone https://github.com/alaingalvan/raw-vulkan --recurse-submodules

# 💿 go inside the folder
cd raw-vulkan

# 👯 If you forget to `recurse-submodules` you can always run:
git submodule update --init

```

## Examples

<!--

All examples share code found `src/Core`, including a Basic Renderer implementation and a variety of utilities.

### Image Renderer

Simply outputs a bitmap generated by the GPU next to the program executable, avoids concepts like synchronization, windowing systems, descriptors sets, uniform buffers, staging buffers, subpass attachments, and depth-stencil buffers.

-->

### Hello triangle

Renders out a triangle to the target OS windowing system and updates it every frame. 

<!--
### ShaderToy Image Shader

Applies a shader to a render pass, with the same inputs as [ShaderToy](https://www.shadertoy.com/)'s image shader. 

### GLTF Scene Renderer

Accepts a GLTF file, and outputs a scene the user can explore.

### VR Render Pass

The Khronos Group recently released a VR initiative and call to action. What this will most likely result in for Vulkan is another layer that developers can include in their project for first class VR support. This project creates geometry similar to [Unreal Engine 4's VR frustrum](https://www.unrealengine.com/blog/unreal-engine-4-10-released) and renders a scene stereoscopically. 

### Anti-Aliasing Showcase

Anti-aliasing is a huge topic of interest for graphics research, here we review multiple methods of anti-aliasing such as:

- FXAA
- MSAA
- TSAA

### Compute Raytracing

An attempt to generate accurate raytracing using Vulkan's Compute Pipeline to get levels of quality similar to Mitsuba's Physically based renderer. 

### Host vs. Device Memory Benchmark

A comparision of rendering using data in *host visible space* vs data in *device visible* space.

### Pipeline Cache Benchmark

A comparison of continuously regenerated Pipelines vs. the use of pipeline caches. A pipeline cache allows the device to store the pipeline in a binary representation for reuse later in uses of the program. 
-->

[cmake-img]: https://img.shields.io/badge/cmake-3.6-1f9948.svg?style=flat-square
[cmake-url]: https://cmake.org/
[license-img]: https://img.shields.io/:license-mit-blue.svg?style=flat-square
[license-url]: https://opensource.org/licenses/MIT
