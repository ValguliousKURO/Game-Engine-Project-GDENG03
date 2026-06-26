#pragma once
#include <DX3D/Core/Core.h>
#include <DX3D/Game/Component.h>
#include <DX3D/Component/PrimitiveMeshData.h>

namespace dx3d
{
    class SphereComponent final : public Component
    {
        dx3d_typeid(SphereComponent)
    public:
        explicit SphereComponent(const ComponentDesc& data);
        static PrimitiveMeshData createMeshData();
    };
}
