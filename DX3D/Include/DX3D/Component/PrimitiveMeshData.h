#pragma once
#include <DX3D/Core/Core.h>
#include <DX3D/Math/Vec2.h>
#include <DX3D/Math/Vec3.h>
#include <DX3D/Math/Vec4.h>
#include <vector>

namespace dx3d
{
	struct PrimitiveVertex
	{
		Vec3 position;
		Vec4 color;
		Vec2 texcoord;
	};

	struct PrimitiveMeshData
	{
		std::vector<PrimitiveVertex> vertices{};
		std::vector<ui32> indices{};
	};
}
