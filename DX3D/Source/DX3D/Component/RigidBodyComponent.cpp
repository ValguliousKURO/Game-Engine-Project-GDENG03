#include <DX3D/Component/RigidBodyComponent.h>
#include <DX3D/Game/World.h>
#include <reactphysics3d/reactphysics3d.h>

dx3d::RigidBodyComponent::RigidBodyComponent(const ComponentDesc& data) : Component(data)
{
}

void dx3d::RigidBodyComponent::configureBox(const Vec3& halfExtents, bool isStatic, f32 mass, f32 bounciness, f32 friction)
{
	m_body = m_world.createPhysicsBox(*this, halfExtents, isStatic, mass, bounciness, friction);
}

void dx3d::RigidBodyComponent::resetTransform(const Vec3& position, const Vec3& rotation)
{
	if (!m_body) return;

	m_body->setTransform({
		reactphysics3d::Vector3(position.x, position.y, position.z),
		reactphysics3d::Quaternion::fromEulerAngles(rotation.x, rotation.y, rotation.z)
	});
	m_body->setLinearVelocity({ 0.0f, 0.0f, 0.0f });
	m_body->setAngularVelocity({ 0.0f, 0.0f, 0.0f });
	m_body->resetForce();
	m_body->resetTorque();
	m_body->setIsSleeping(false);
}

reactphysics3d::RigidBody* dx3d::RigidBodyComponent::getBody() const noexcept
{
	return m_body;
}

bool dx3d::RigidBodyComponent::isConfigured() const noexcept
{
	return m_body != nullptr;
}
