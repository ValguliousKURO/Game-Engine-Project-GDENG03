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
#include <DX3D/Component/ModelComponent.h>
#include <DX3D/Component/CameraComponent.h>

#include <stb_image.h>
#include <algorithm>
#include <array>
#include <cfloat>
#include <fstream>
#include <sstream>
#include <ranges>
#include <stdexcept>
#include <string>


namespace
{
	struct ObjFaceVertex
	{
		int position{};
		int texcoord{};
	};

	int parseObjIndex(const std::string& value, int count)
	{
		if (value.empty()) return 0;
		const auto index = std::stoi(value);
		return index > 0 ? index - 1 : count + index;
	}

	ObjFaceVertex parseFaceVertex(const std::string& token, int positionCount, int texcoordCount)
	{
		ObjFaceVertex vertex{};
		const auto firstSlash = token.find('/');
		if (firstSlash == std::string::npos)
		{
			vertex.position = parseObjIndex(token, positionCount);
			vertex.texcoord = -1;
			return vertex;
		}

		vertex.position = parseObjIndex(token.substr(0, firstSlash), positionCount);
		const auto secondSlash = token.find('/', firstSlash + 1);
		const auto texcoordText = token.substr(firstSlash + 1, secondSlash - firstSlash - 1);
		vertex.texcoord = texcoordText.empty() ? -1 : parseObjIndex(texcoordText, texcoordCount);
		return vertex;
	}

	dx3d::PrimitiveMeshData loadObjMesh(const char* filename, const dx3d::Vec4& color)
	{
		std::ifstream file(filename);
		if (!file) throw std::runtime_error("Failed to open OBJ file.");

		std::vector<dx3d::Vec3> positions{};
		std::vector<dx3d::Vec2> texcoords{};
		dx3d::PrimitiveMeshData data{};

		dx3d::Vec3 minBounds{ FLT_MAX, FLT_MAX, FLT_MAX };
		dx3d::Vec3 maxBounds{ -FLT_MAX, -FLT_MAX, -FLT_MAX };

		std::string line{};
		while (std::getline(file, line))
		{
			std::istringstream stream(line);
			std::string prefix{};
			stream >> prefix;

			if (prefix == "v")
			{
				dx3d::Vec3 position{};
				stream >> position.x >> position.y >> position.z;
				positions.push_back(position);
				minBounds.x = std::min(minBounds.x, position.x);
				minBounds.y = std::min(minBounds.y, position.y);
				minBounds.z = std::min(minBounds.z, position.z);
				maxBounds.x = std::max(maxBounds.x, position.x);
				maxBounds.y = std::max(maxBounds.y, position.y);
				maxBounds.z = std::max(maxBounds.z, position.z);
			}
			else if (prefix == "vt")
			{
				dx3d::Vec2 texcoord{};
				stream >> texcoord.x >> texcoord.y;
				texcoords.push_back(texcoord);
			}
			else if (prefix == "f")
			{
				std::vector<ObjFaceVertex> face{};
				std::string token{};
				while (stream >> token)
				{
					face.push_back(parseFaceVertex(token, static_cast<int>(positions.size()), static_cast<int>(texcoords.size())));
				}

				for (auto i = 1u; i + 1u < face.size(); ++i)
				{
					const std::array<ObjFaceVertex, 3> triangle{ face[0], face[i], face[i + 1u] };
					for (const auto& vertex : triangle)
					{
						if (vertex.position < 0 || vertex.position >= static_cast<int>(positions.size())) continue;
						const auto position = positions[vertex.position];
						dx3d::Vec2 texcoord{};
						if (vertex.texcoord >= 0 && vertex.texcoord < static_cast<int>(texcoords.size()))
						{
							texcoord = texcoords[vertex.texcoord];
							texcoord.y = 1.0f - texcoord.y;
						}
						else
						{
							const auto width = std::max(maxBounds.x - minBounds.x, 0.0001f);
							const auto depth = std::max(maxBounds.z - minBounds.z, 0.0001f);
							texcoord = { (position.x - minBounds.x) / width, (position.z - minBounds.z) / depth };
						}

						data.indices.push_back(static_cast<dx3d::ui32>(data.vertices.size()));
						data.vertices.push_back({ position, color, texcoord });
					}
				}
			}
		}

		const auto center = dx3d::Vec3{
			(minBounds.x + maxBounds.x) * 0.5f,
			(minBounds.y + maxBounds.y) * 0.5f,
			(minBounds.z + maxBounds.z) * 0.5f
		};
		const auto size = std::max({ maxBounds.x - minBounds.x, maxBounds.y - minBounds.y, maxBounds.z - minBounds.z, 0.0001f });
		const auto scale = 1.0f / size;

		for (auto& vertex : data.vertices)
		{
			vertex.position = {
				(vertex.position.x - center.x) * scale,
				(vertex.position.y - center.y) * scale,
				(vertex.position.z - center.z) * scale
			};
		}

		return data;
	}

	dx3d::WorldRenderer::Mesh createMesh(dx3d::GraphicsDevice& device, const dx3d::PrimitiveMeshData& data)
	{
		return {
			device.createVertexBuffer({ data.vertices.data(), static_cast<dx3d::ui32>(data.vertices.size()), sizeof(dx3d::PrimitiveVertex) }),
			device.createIndexBuffer({ data.indices.data(), static_cast<dx3d::ui32>(data.indices.size()) })
		};
	}

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> createTexture(dx3d::GraphicsDevice& device, const char* filename)
	{
		int width{};
		int height{};
		auto imageData = stbi_load(filename, &width, &height, nullptr, 4);
		if (!imageData) throw std::runtime_error("Failed to load texture file.");

		D3D11_TEXTURE2D_DESC textureDesc{};
		textureDesc.Width = static_cast<UINT>(width);
		textureDesc.Height = static_cast<UINT>(height);
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		D3D11_SUBRESOURCE_DATA textureData{};
		textureData.pSysMem = imageData;
		textureData.SysMemPitch = static_cast<UINT>(width * 4);

		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture{};
		auto hr = device.getD3DDevice()->CreateTexture2D(&textureDesc, &textureData, &texture);
		stbi_image_free(imageData);
		if (FAILED(hr)) throw std::runtime_error("CreateTexture2D failed.");

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv{};
		if (FAILED(device.getD3DDevice()->CreateShaderResourceView(texture.Get(), &srvDesc, &srv)))
		{
			throw std::runtime_error("CreateShaderResourceView failed.");
		}
		return srv;
	}

	Microsoft::WRL::ComPtr<ID3D11SamplerState> createSampler(dx3d::GraphicsDevice& device)
	{
		D3D11_SAMPLER_DESC desc{};
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MinLOD = 0;
		desc.MaxLOD = D3D11_FLOAT32_MAX;

		Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler{};
		if (FAILED(device.getD3DDevice()->CreateSamplerState(&desc, &sampler)))
		{
			throw std::runtime_error("CreateSamplerState failed.");
		}
		return sampler;
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
	m_teapotMesh = createMesh(device, loadObjMesh("teapot.obj", { 1.0f, 1.0f, 1.0f, 1.0f }));
	m_bunnyMesh = createMesh(device, loadObjMesh("bunny.obj", { 0.78f, 0.82f, 0.88f, 1.0f }));
	m_armadilloMesh = createMesh(device, loadObjMesh("armadillo.obj", { 0.82f, 0.70f, 0.56f, 1.0f }));
	m_brickTexture = createTexture(device, "brick.png");
	m_sampler = createSampler(device);
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


	auto drawMesh = [&context, this, &data](const Mesh& mesh, TransformComponent& transform, const Vec4& tint, bool useTexture)
	{
		data.world = transform.getAffineWorldMatrix();
		data.tint = tint;
		data.useTexture = useTexture ? 1.0f : 0.0f;

		auto& cb = *m_cb;
		context.updateConstantBuffer(cb, &data);
		context.setVertexBuffer(*mesh.vertexBuffer);
		context.setConstantBuffer(cb);
		context.setIndexBuffer(*mesh.indexBuffer);
		context.setTexture(useTexture ? m_brickTexture.Get() : nullptr);
		context.setSampler(m_sampler.Get());
		context.drawIndexedTriangleList(mesh.indexBuffer->getIndexListSize(), 0u, 0u);
	};

	auto drawComponents = [&world, &numComponents, &drawMesh]<typename ComponentType>(const Mesh& mesh)
	{
		auto components = world.getComponents<ComponentType>(numComponents);

		for (auto i : std::views::iota(0u, numComponents))
		{
			auto component = components[i];
			auto& transform = component->getGameObject().getTransform();
			drawMesh(mesh, transform, { 1.0f, 1.0f, 1.0f, 1.0f }, false);
		}
	};

	drawComponents.operator()<PlaneComponent>(m_planeMesh);
	drawComponents.operator()<CubeComponent>(m_cubeMesh);
	drawComponents.operator()<SphereComponent>(m_sphereMesh);

	auto modelComponents = world.getComponents<ModelComponent>(numComponents);
	for (auto i : std::views::iota(0u, numComponents))
	{
		auto component = modelComponents[i];
		auto& transform = component->getGameObject().getTransform();
		switch (component->getModelType())
		{
		case ModelType::Teapot:
			drawMesh(m_teapotMesh, transform, component->getTint(), true);
			break;
		case ModelType::Bunny:
			drawMesh(m_bunnyMesh, transform, component->getTint(), false);
			break;
		case ModelType::Armadillo:
			drawMesh(m_armadilloMesh, transform, component->getTint(), false);
			break;
		}
	}

	m_graphicsDevice.executeCommandList(context);

	// Ensure the back buffer is set as the active render target on the immediate context
	auto rtv = swapChain.getRenderTargetView();
	m_graphicsDevice.getD3DDeviceContext()->OMSetRenderTargets(1, &rtv, nullptr);

	// Draw ImGui onto the back buffer (using immediate context)
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	swapChain.present();
}
