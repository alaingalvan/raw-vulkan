#pragma once

#include <vulkan/vulkan.hpp>

namespace raw
{
	struct Vec2
	{
		float x;
		float y;
	};

	struct Vec3
	{
		float x;
		float y;
		float z;
	};

    struct MeshData
    {
        
	std::vector<Vec3> position;

	std::vector<Vec3> normals;

	std::vector<Vec3> tangents;

	std::vector<Vec3> bitangents;

	//std::vector<Vec4> colors;

	std::vector<Vec2> textureCoords;

	std::vector<unsigned> indices;
    }

    struct MeshRasterBuffers
    {
        vk::Buffer indices;
        vk::Buffer positions;
        vk::Buffer normals;
        vk::Buffer tangents;
        vk::Buffer bitangents;
        vk::Buffer texCoords;

    };

    // Shader
struct Shader
{
	unsigned vertexShader = 0;
	unsigned fragmentShader = 0;
	unsigned program = 0;
};

struct StandardMaterial
{
	struct VertexUBO
	{
		Matrix4 modelViewProjection = Matrix4::identity();
		Matrix4 inverseTransposeModel = Matrix4::identity();
	};
	VertexUBO vertexUboData;
	unsigned vertexUbo = 0;
	unsigned vertexUbi = 0;

	struct FragUBO
	{
		Vector3 cameraPosition = Vector3(4.0f, 4.0f, 4.0f);
	};
	FragUBO fragmentUboData;
	unsigned fragmentUbo = 0;
	unsigned fragmentUbi = 0;
	Shader shader;


};

struct Mesh
{
	MeshData data;
	MeshBuffers buffers;
	VertexLayout layout;
	StandardMaterial material;

	std::vector<Bone> skeleton;
	//skin, bones, etc.

	void bindVertexLayout();

	void bindBuffers();

	void bindShader();

	void bindUniforms();

	void draw();
};


}