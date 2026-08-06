#include <DX3D/Component/PlaneComponent.h>

dx3d::PlaneComponent::PlaneComponent(const ComponentDesc& data) : Component(data)
{
}

dx3d::PrimitiveMeshData dx3d::PlaneComponent::createMeshData()
{
    return {
        {
            {{-0.5f,0.0f,-0.5f}, {0.2f,0.55f,0.35f,1.0f}, {0.0f,1.0f}},
            {{-0.5f,0.0f,0.5f}, {0.25f,0.65f,0.4f,1.0f}, {0.0f,0.0f}},
            {{0.5f,0.0f,0.5f}, {0.35f,0.7f,0.45f,1.0f}, {1.0f,0.0f}},
            {{0.5f,0.0f,-0.5f}, {0.2f,0.5f,0.35f,1.0f}, {1.0f,1.0f}}
        },
        {
            0,1,2,
            2,3,0
        }
    };
}
