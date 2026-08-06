#include <DX3D/Component/ModelComponent.h>

dx3d::ModelComponent::ModelComponent(const ComponentDesc& data) : Component(data)
{
}

void dx3d::ModelComponent::setModelType(ModelType type) noexcept
{
	m_type = type;
}

dx3d::ModelType dx3d::ModelComponent::getModelType() const noexcept
{
	return m_type;
}

void dx3d::ModelComponent::setTint(const Vec4& tint) noexcept
{
	m_tint = tint;
}

const dx3d::Vec4& dx3d::ModelComponent::getTint() const noexcept
{
	return m_tint;
}
