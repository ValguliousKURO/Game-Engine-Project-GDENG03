#include <DX3D/Component/SphereComponent.h>
#include <cmath>

dx3d::SphereComponent::SphereComponent(const ComponentDesc& data) : Component(data)
{
}

dx3d::PrimitiveMeshData dx3d::SphereComponent::createMeshData()
{
    constexpr auto pi = 3.14159265359f;
    constexpr dx3d::ui32 slices = 24;
    constexpr dx3d::ui32 stacks = 16;

    dx3d::PrimitiveMeshData data{};

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
