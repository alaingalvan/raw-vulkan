# Raw Vulkan Examples

![Release][release-img] [![License][license-img]][license-url]

A number of Vulkan examples developed as exercises to understand the API. Powered by the [LunarG Vulkan SDK](https://vulkan.lunarg.com/), [GLM](https://github.com/g-truc/glm).

> At the moment, only the Hello Triangle is complete!

## Setup

This project uses [Conan](http://conan.io) as a C++ package manager. Install Conan, [review the Conan docs](http://conanio.readthedocs.io/en/latest/integrations.html) for your IDE's integration, then type the following in a console:

```bash
conan install --build
```

## Image Renderer

The simplest possible Vulkan application. Simply outputs a bitmap image file of a hello triangle next to the program.

## Hello triangle

Renders out a triangle to the target OS windowing system and updates it every frame.

## ShaderToy Image Shader

Applies a shader to a render pass, with the same inputs as [ShaderToy](https://www.shadertoy.com/)'s image shader. 

## GLTF Scene Renderer

Accepts a GLTF file, and outputs a scene the user can explore.

## VR Render pass

The Khronos Group recently released a VR initiative and call to action. What this will most likely result in for Vulkan is another layer that developers can include in their project for first class VR support. This project creates geometry similar to [Unreal Engine 4's VR frustrum](https://www.unrealengine.com/blog/unreal-engine-4-10-released) and renders a scene stereoscopically. 

## Anti-Aliasing Showcase

Anti-aliasing is a huge topic of interest for graphics research, here we review multiple methods of anti-aliasing such as:

- FXAA
- MSAA
- TSAA

## Compute Raytracing

An attempt to generate accurate raytracing using Vulkan's Compute Pipeline to get levels of quality similar to Mitsuba's Physically based renderer. 

[release-img]: https://img.shields.io/badge/release-1.0.0.0-B46BD6.svg?style=flat-square
[license-img]: http://img.shields.io/:license-mit-blue.svg?style=flat-square
[license-url]: https://opensource.org/licenses/MIT