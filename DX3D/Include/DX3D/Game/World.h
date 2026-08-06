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

#pragma once
#include <DX3D/Core/Common.h>
#include <DX3D/Core/Base.h>
#include <DX3D/Core/Identifiable.h>
#include <DX3D/Math/Vec3.h>
#include <unordered_map>
#include <vector>

namespace reactphysics3d
{
	class PhysicsCommon;
	class PhysicsWorld;
	class BoxShape;
	class RigidBody;
}

namespace dx3d
{
	class RigidBodyComponent;

	class World final : public Base
	{
	public:
		explicit World(const WorldDesc& desc);
		virtual ~World() override;

		template <typename T>
		T* createGameObject() requires IsRegistered<GameObject, T>
		{
			UniquePtr<GameObject> e = std::make_unique<T>(GameObjectDesc{
				{m_logger},
				m_gameContext,
				*this
				});
			return static_cast<T*>(createGameObjectInternal(e));
		}

		template <typename T> requires IsRegistered<Component, T>
		T* const* getComponents(ui32& numComponents) const noexcept
		{
			return reinterpret_cast<T* const*>(getComponentsInternal(T::GetTypeId(), &numComponents));
		}

		void update(f32 deltaTime);
		void setPhysicsEnabled(bool enabled) noexcept;
		bool isPhysicsEnabled() const noexcept;
		reactphysics3d::RigidBody* createPhysicsBox(RigidBodyComponent& component, const Vec3& halfExtents, bool isStatic,
			f32 mass, f32 bounciness, f32 friction);
	private:
		GameObject* createGameObjectInternal(UniquePtr<GameObject>& object);
		void addComponentInternal(Component& component);
		void addDirtyTransformInternal(TransformComponent& component);
		void stepPhysics(f32 deltaTime);
		void syncPhysicsTransforms();

		Component* const* getComponentsInternal(size_t typeId, ui32* numComponents) const noexcept;
	private:
		enum class EventType
		{
			Create = 0
		};
		struct GameObjectEvent
		{
			GameObject* object{};
			size_t pendingObjectIndex{};
			EventType eventType{};
		};

	private:
		GameContext m_gameContext;

		std::unordered_map<size_t, std::vector<UniquePtr<GameObject>>> m_objects{};
		std::unordered_map<size_t, std::vector<Component*>> m_components{};

		std::vector<TransformComponent*> m_dirtyTransforms{};

		std::vector<UniquePtr<GameObject>> m_pendingObjects;
		std::vector<UniquePtr<GameObject>> m_pendingObjectsSwapBuffer;

		std::vector<GameObjectEvent> m_events{};
		std::vector<GameObjectEvent> m_eventsSwapBuffer{};

		UniquePtr<reactphysics3d::PhysicsCommon> m_physicsCommon{};
		reactphysics3d::PhysicsWorld* m_physicsWorld{};
		std::vector<reactphysics3d::BoxShape*> m_physicsBoxShapes{};
		f32 m_physicsAccumulator{};
		bool m_physicsEnabled{ false };

		friend class GameObject;
		friend class TransformComponent;
	};
}

