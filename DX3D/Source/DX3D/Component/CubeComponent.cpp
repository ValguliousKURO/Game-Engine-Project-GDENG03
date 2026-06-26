#include <DX3D/Component/CubeComponent.h>
#include <DX3D/Game/World.h>

dx3d::CubeComponent::CubeComponent(const ComponentDesc& data) : Component(data)
{
}

dx3d::PrimitiveMeshData dx3d::CubeComponent::createMeshData()
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
