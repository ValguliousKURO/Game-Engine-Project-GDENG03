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

#include <DX3D/Game/World.h>
#include <DX3D/Game/GameObject.h>
#include <DX3D/Game/Component.h>
#include <DX3D/Component/TransformComponent.h>
#include <DX3D/Component/RigidBodyComponent.h>

#include <reactphysics3d/reactphysics3d.h>
#include <algorithm>
#include <cmath>

dx3d::World::World(const WorldDesc& desc) : Base(desc.base), m_gameContext(desc.gameContext)
{
	m_physicsCommon = std::make_unique<reactphysics3d::PhysicsCommon>();
	reactphysics3d::PhysicsWorld::WorldSettings settings{};
	settings.gravity = reactphysics3d::Vector3(0.0f, -9.81f, 0.0f);
	m_physicsWorld = m_physicsCommon->createPhysicsWorld(settings);
}

dx3d::World::~World()
{
	if (m_physicsCommon && m_physicsWorld)
	{
		m_physicsCommon->destroyPhysicsWorld(m_physicsWorld);
		m_physicsWorld = nullptr;
	}

	if (m_physicsCommon)
	{
		for (auto shape : m_physicsBoxShapes)
		{
			m_physicsCommon->destroyBoxShape(shape);
		}
		m_physicsBoxShapes.clear();
	}
}

void dx3d::World::update(f32 deltaTime)
{
	if (m_events.size())
	{
		std::swap(m_events, m_eventsSwapBuffer);
		std::swap(m_pendingObjects, m_pendingObjectsSwapBuffer);
	
		for (auto& e : m_eventsSwapBuffer)
		{
			auto objTypeId = e.object->getTypeId();
			auto pendingObjIndex = e.pendingObjectIndex;

			if (e.eventType == EventType::Create)
			{
				auto& obj = m_pendingObjectsSwapBuffer[pendingObjIndex];
				auto ptr = obj.get();
				m_objects[objTypeId].push_back(std::move(obj));
				ptr->onCreate();		
			}
		}

		m_pendingObjectsSwapBuffer.clear();
		m_eventsSwapBuffer.clear();
	}

	for (auto&& [typeId, objects] : m_objects)
	{
		for (auto& object : objects)
		{
			object->onUpdate(deltaTime);
		}
	}

	if (m_physicsEnabled)
	{
		stepPhysics(deltaTime);
		syncPhysicsTransforms();
	}

	for (auto& comp : m_dirtyTransforms)
	{
		comp->updateWorldMatrix();
	}
	m_dirtyTransforms.clear();
}

dx3d::GameObject* dx3d::World::createGameObjectInternal(UniquePtr<GameObject>& object)
{
	if (!object) return {};

	auto ptr = object.get();

	auto index = m_pendingObjects.size();
	m_pendingObjects.push_back(std::move(object));
	m_events.push_back({ ptr, index, EventType::Create });

	return ptr;
}

void dx3d::World::addComponentInternal(Component& component)
{
	auto typeId = component.getTypeId();
	m_components[typeId].push_back(&component);
}

void dx3d::World::addDirtyTransformInternal(TransformComponent& component)
{
	m_dirtyTransforms.push_back(&component);
}

dx3d::Component* const* dx3d::World::getComponentsInternal(size_t typeId, ui32* numComponents) const noexcept
{
	auto it = m_components.find(typeId);
	if (it != m_components.end())
	{
		*numComponents = static_cast<ui32>(it->second.size());
		return it->second.data();
	}

	*numComponents = 0u;
	return {};
}

void dx3d::World::setPhysicsEnabled(bool enabled) noexcept
{
	m_physicsEnabled = enabled;
	if (!enabled)
	{
		m_physicsAccumulator = 0.0f;
	}
}

bool dx3d::World::isPhysicsEnabled() const noexcept
{
	return m_physicsEnabled;
}

reactphysics3d::RigidBody* dx3d::World::createPhysicsBox(RigidBodyComponent& component, const Vec3& halfExtents, bool isStatic,
	f32 mass, f32 bounciness, f32 friction)
{
	if (!m_physicsWorld || !m_physicsCommon) return {};

	auto& transform = component.getGameObject().getTransform();
	const auto position = transform.getPosition();
	const auto rotation = transform.getRotation();

	reactphysics3d::Transform bodyTransform{
		reactphysics3d::Vector3(position.x, position.y, position.z),
		reactphysics3d::Quaternion::fromEulerAngles(rotation.x, rotation.y, rotation.z)
	};

	auto body = m_physicsWorld->createRigidBody(bodyTransform);
	body->setType(isStatic ? reactphysics3d::BodyType::STATIC : reactphysics3d::BodyType::DYNAMIC);
	body->setIsAllowedToSleep(false);

	auto shape = m_physicsCommon->createBoxShape(reactphysics3d::Vector3(halfExtents.x, halfExtents.y, halfExtents.z));
	m_physicsBoxShapes.push_back(shape);

	auto collider = body->addCollider(shape, reactphysics3d::Transform::identity());
	auto& material = collider->getMaterial();
	material.setBounciness(bounciness);
	material.setFrictionCoefficient(friction);

	if (!isStatic)
	{
		body->updateMassPropertiesFromColliders();
		body->setMass(mass);
	}

	return body;
}

void dx3d::World::stepPhysics(f32 deltaTime)
{
	if (!m_physicsWorld) return;

	constexpr f32 fixedTimeStep = 1.0f / 60.0f;
	m_physicsAccumulator += std::min(deltaTime, 0.1f);

	while (m_physicsAccumulator >= fixedTimeStep)
	{
		m_physicsWorld->update(fixedTimeStep);
		m_physicsAccumulator -= fixedTimeStep;
	}
}

namespace
{
	dx3d::Vec3 toEulerAngles(const reactphysics3d::Quaternion& q)
	{
		const auto sinrCosp = 2.0f * (q.w * q.x + q.y * q.z);
		const auto cosrCosp = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
		const auto x = std::atan2(sinrCosp, cosrCosp);

		const auto sinp = 2.0f * (q.w * q.y - q.z * q.x);
		const auto y = std::abs(sinp) >= 1.0f ? std::copysign(1.57079632679f, sinp) : std::asin(sinp);

		const auto sinyCosp = 2.0f * (q.w * q.z + q.x * q.y);
		const auto cosyCosp = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
		const auto z = std::atan2(sinyCosp, cosyCosp);

		return { x, y, z };
	}
}

void dx3d::World::syncPhysicsTransforms()
{
	auto numComponents = 0u;
	auto components = getComponents<RigidBodyComponent>(numComponents);
	for (auto i = 0u; i < numComponents; ++i)
	{
		auto component = components[i];
		auto body = component->getBody();
		if (!body || body->getType() == reactphysics3d::BodyType::STATIC) continue;

		const auto& physicsTransform = body->getTransform();
		const auto& position = physicsTransform.getPosition();
		const auto& orientation = physicsTransform.getOrientation();

		auto& transform = component->getGameObject().getTransform();
		transform.setPosition({ position.x, position.y, position.z });
		transform.setRotation(toEulerAngles(orientation));
	}
}
