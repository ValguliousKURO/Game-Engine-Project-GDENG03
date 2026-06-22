

#pragma once
#include <DX3D/Core/Core.h>
#include <DX3D/Core/Base.h>
#include <DX3D/Math/Vec3.h>
#include <DX3D/Math/Vec4.h>
#include <DX3D/Math/Mat4x4.h>

namespace dx3d
{
	class WorldRenderer final : public Base
	{
	public:
		explicit WorldRenderer(const WorldRendererDesc& desc);
		virtual ~WorldRenderer() override;

		void render(const World& world, SwapChain& swapChain, f32 deltaTime);

		struct Vertex
		{
			Vec3 position;
			Vec4 color;
		};
		struct Mesh
		{
			RefPtr<VertexBuffer> vertexBuffer{};
			RefPtr<IndexBuffer> indexBuffer{};
		};
		struct alignas(16) ConstantData
		{
			Mat4x4 world{};
			Mat4x4 view{};
			Mat4x4 proj{};
		};

	private:
		GraphicsDevice& m_graphicsDevice;
		RefPtr<DeviceContext> m_deviceContext{};
		RefPtr<GraphicsPipelineState> m_pipeline{};
		RefPtr<ConstantBuffer> m_cb{};
		Mesh m_circleMesh{};
		Mesh m_cubeMesh{};
		Mesh m_planeMesh{};
		Mesh m_sphereMesh{};
	};
}

