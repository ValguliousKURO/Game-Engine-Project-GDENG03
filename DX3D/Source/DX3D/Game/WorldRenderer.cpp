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

#include <imgui.h>
#include <backends/imgui_impl_dx11.h>

#include <DX3D/Game/World.h>
#include <DX3D/Game/Component.h>
#include <DX3D/Game/GameObject.h>

#include <DX3D/Component/TransformComponent.h>
#include <DX3D/Component/CubeComponent.h>
#include <DX3D/Component/PlaneComponent.h>
#include <DX3D/Component/SphereComponent.h>
#include <DX3D/Component/CameraComponent.h>

#include <fstream>
#include <ranges>


namespace
{
	dx3d::WorldRenderer::Mesh createMesh(dx3d::GraphicsDevice& device, const dx3d::PrimitiveMeshData& data)
	{
		return {
			device.createVertexBuffer({ data.vertices.data(), static_cast<dx3d::ui32>(data.vertices.size()), sizeof(dx3d::PrimitiveVertex) }),
			device.createIndexBuffer({ data.indices.data(), static_cast<dx3d::ui32>(data.indices.size()) })
		};
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
	m_cubeMesh = createMesh(device, CubeComponent::createMeshData());
	m_planeMesh = createMesh(device, PlaneComponent::createMeshData());
	m_sphereMesh = createMesh(device, SphereComponent::createMeshData());
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

	// Ensure the back buffer is set as the active render target on the immediate context
	auto rtv = swapChain.getRenderTargetView();
	m_graphicsDevice.getD3DDeviceContext()->OMSetRenderTargets(1, &rtv, nullptr);

	// Draw ImGui onto the back buffer (using immediate context)
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	swapChain.present();
}
