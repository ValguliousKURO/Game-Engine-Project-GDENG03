#include <DX3D/Component/PlaneComponent.h>

dx3d::PlaneComponent::PlaneComponent(const ComponentDesc& data) : Component(data)
{
}

dx3d::PrimitiveMeshData dx3d::PlaneComponent::createMeshData()
{
    return {
        {
            {{-0.5f,0.0f,-0.5f}, {1.0f,1.0f,1.0f,1.0f}},
            {{-0.5f,0.0f,0.5f}, {1.0f,1.0f,1.0f,1.0f}},
            {{0.5f,0.0f,0.5f}, {1.0f,1.0f,1.0f,1.0f}},
            {{0.5f,0.0f,-0.5f}, {1.0f,1.0f,1.0f,1.0f}}
        },
        {
            0,1,2,
            2,3,0
        }
    };
}
