/*MIT License

C++ 3D Game Tutorial Series (https://github.com/PardCode/CPP-3D-Game-Tutorial-Series)

Copyright (c) 2019-2026, PardCode

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#include <DX3D/Game/WorldRenderer.h>
#include <DX3D/Graphics/GraphicsDevice.h>
#include <DX3D/Graphics/DeviceContext.h>
#include <DX3D/Graphics/SwapChain.h>
#include <DX3D/Graphics/VertexBuffer.h>
#include <DX3D/Graphics/IndexBuffer.h>

#include <DX3D/Game/World.h>
#include <DX3D/Game/Component.h>
#include <DX3D/Game/GameObject.h>

#include <DX3D/Component/TransformComponent.h>
#include <DX3D/Component/CubeComponent.h>
#include <DX3D/Component/PlaneComponent.h>
#include <DX3D/Component/SphereComponent.h>
#include <DX3D/Component/CameraComponent.h>

#include <DX3D/Math/Vec3.h>
#include <fstream>
#include <ranges>
#include <vector>
#include <cmath>


namespace
{
	struct MeshData
	{
		std::vector<dx3d::WorldRenderer::Vertex> vertices{};
		std::vector<dx3d::ui32> indices{};
	};

	dx3d::WorldRenderer::Mesh createMesh(dx3d::GraphicsDevice& device, const MeshData& data)
	{
		return {
			device.createVertexBuffer({ data.vertices.data(), static_cast<dx3d::ui32>(data.vertices.size()), sizeof(dx3d::WorldRenderer::Vertex) }),
			device.createIndexBuffer({ data.indices.data(), static_cast<dx3d::ui32>(data.indices.size()) })
		};
	}

	MeshData createCubeMeshData()
	{
		return {
			{
				{{-0.5f,-0.5f,-0.5f}, {1.0f,0.1f,0.1f,1.0f}},
				{{-0.5f,0.5f,-0.5f}, {0.1f,0.8f,0.2f,1.0f}},
				{{0.5f,0.5f,-0.5f}, {0.1f,0.3f,1.0f,1.0f}},
				{{0.5f,-0.5f,-0.5f}, {1.0f,0.9f,0.1f,1.0f}},

				{{0.5f,-0.5f,0.5f}, {1.0f,0.4f,0.1f,1.0f}},
				{{0.5f,0.5f,0.5f}, {0.2f,0.9f,1.0f,1.0f}},
				{{-0.5f,0.5f,0.5f}, {0.8f,0.2f,1.0f,1.0f}},
				{{-0.5f,-0.5f,0.5f}, {0.9f,0.9f,0.9f,1.0f}}
			},
			{
				0,1,2, 2,3,0,
				4,5,6, 6,7,4,
				1,6,5, 5,2,1,
				7,0,3, 3,4,7,
				3,2,5, 5,4,3,
				7,6,1, 1,0,7
			}
		};
	}

	MeshData createPlaneMeshData()
	{
		return {
			{
				{{-0.5f,0.0f,-0.5f}, {0.2f,0.55f,0.35f,1.0f}},
				{{-0.5f,0.0f,0.5f}, {0.25f,0.65f,0.4f,1.0f}},
				{{0.5f,0.0f,0.5f}, {0.35f,0.7f,0.45f,1.0f}},
				{{0.5f,0.0f,-0.5f}, {0.2f,0.5f,0.35f,1.0f}}
			},
			{
				0,1,2,
				2,3,0
			}
		};
	}

	MeshData createSphereMeshData()
	{
		constexpr auto pi = 3.14159265359f;
		constexpr dx3d::ui32 slices = 24;
		constexpr dx3d::ui32 stacks = 16;

		MeshData data{};

		for (auto stack = 0u; stack <= stacks; ++stack)
		{
			const auto v = static_cast<dx3d::f32>(stack) / static_cast<dx3d::f32>(stacks);
			const auto phi = v * pi;
			const auto y = std::cos(phi) * 0.5f;
			const auto radius = std::sin(phi) * 0.5f;

			for (auto slice = 0u; slice <= slices; ++slice)
			{
				const auto u = static_cast<dx3d::f32>(slice) / static_cast<dx3d::f32>(slices);
				const auto theta = u * pi * 2.0f;
				const auto x = std::cos(theta) * radius;
				const auto z = std::sin(theta) * radius;

				data.vertices.push_back({
					{x, y, z},
					{0.35f + (0.55f * u), 0.45f + (0.35f * v), 1.0f - (0.45f * v), 1.0f}
				});
			}
		}

		for (auto stack = 0u; stack < stacks; ++stack)
		{
			for (auto slice = 0u; slice < slices; ++slice)
			{
				const auto rowA = stack * (slices + 1);
				const auto rowB = (stack + 1) * (slices + 1);

				data.indices.push_back(rowA + slice);
				data.indices.push_back(rowB + slice);
				data.indices.push_back(rowA + slice + 1);

				data.indices.push_back(rowA + slice + 1);
				data.indices.push_back(rowB + slice);
				data.indices.push_back(rowB + slice + 1);
			}
		}

		return data;
	}
}

dx3d::WorldRenderer::WorldRenderer(const WorldRendererDesc& desc) : Base(desc.base), m_graphicsDevice(desc.engine)
{
	auto& device = m_graphicsDevice;
	m_deviceContext = device.createDeviceContext();

	constexpr char shaderFilePath[] = "DX3D/Assets/Shaders/Basic.hlsl";
	std::ifstream shaderStream(shaderFilePath);
	if (!shaderStream) DX3DLogThrowError("Failed to open shader file.");
	std::string shaderFileData{
		std::istreambuf_iterator<char>(shaderStream),
		std::istreambuf_iterator<char>()
	};

	auto shaderSourceCode = shaderFileData.c_str();
	auto shaderSourceCodeSize = shaderFileData.length();

	auto vs = device.compileShader({ shaderFilePath, shaderSourceCode, shaderSourceCodeSize,
		"VSMain", ShaderType::VertexShader });
	auto ps = device.compileShader({ shaderFilePath, shaderSourceCode, shaderSourceCodeSize,
		"PSMain", ShaderType::PixelShader });
	auto vsSig = device.createVertexShaderSignature({ vs });

	m_pipeline = device.createGraphicsPipelineState({ *vsSig, *ps });

	m_cb = device.createConstantBuffer({ {}, sizeof(ConstantData) });
	m_cubeMesh = createMesh(device, createCubeMeshData());
	m_planeMesh = createMesh(device, createPlaneMeshData());
	m_sphereMesh = createMesh(device, createSphereMeshData());
}

dx3d::WorldRenderer::~WorldRenderer()
{
}

void dx3d::WorldRenderer::render(const World& world, SwapChain& swapChain, f32 deltaTime)
{
	auto size = swapChain.getSize();

	auto& context = *m_deviceContext;
	context.clearAndSetBackBuffer(swapChain, { 0.27f, 0.39f, 0.55f, 1.0f });
	context.setGraphicsPipelineState(*m_pipeline);
	context.setViewportSize(size);

	auto numComponents = 0u;
	ConstantData data{};
	{
		auto components = world.getComponents<CameraComponent>(numComponents);
		for (auto i : std::views::iota(0u, numComponents))
		{
			auto component = components[i];
			data.view = component->getViewMatrix();
			component->setViewportSize(size);
			data.proj = component->getProjectionMatrix();
			break;
		}
	}


	auto drawMesh = [&context, this, &data](const Mesh& mesh, TransformComponent& transform)
	{
		data.world = transform.getAffineWorldMatrix();

		auto& cb = *m_cb;
		context.updateConstantBuffer(cb, &data);
		context.setVertexBuffer(*mesh.vertexBuffer);
		context.setConstantBuffer(cb);
		context.setIndexBuffer(*mesh.indexBuffer);
		context.drawIndexedTriangleList(mesh.indexBuffer->getIndexListSize(), 0u, 0u);
	};

	auto drawComponents = [&world, &numComponents, &drawMesh]<typename ComponentType>(const Mesh& mesh)
	{
		auto components = world.getComponents<ComponentType>(numComponents);

		for (auto i : std::views::iota(0u, numComponents))
		{
			auto component = components[i];
			auto& transform = component->getGameObject().getTransform();
			drawMesh(mesh, transform);
		}
	};

	drawComponents.operator()<PlaneComponent>(m_planeMesh);
	drawComponents.operator()<CubeComponent>(m_cubeMesh);
	drawComponents.operator()<SphereComponent>(m_sphereMesh);


	m_graphicsDevice.executeCommandList(context);
	swapChain.present();
}
