#include "Player.h"
#include <Windows.h>
#include <imgui.h>

namespace
{
	class MoveCommand final : public dx3d::InputCommand
	{
	public:
		MoveCommand(Player& player, const dx3d::Vec3& direction, dx3d::f32 speed) :
			m_player(player),
			m_direction(direction),
			m_speed(speed)
		{
		}

		virtual void execute(dx3d::f32 deltaTime) override
		{
			if (ImGui::GetIO().WantCaptureKeyboard)
				return;

			auto& transform = m_player.getTransform();
			auto movement = transform.forward() * m_direction.z +
				transform.right() * m_direction.x;

			if (movement.x == 0.0f && movement.y == 0.0f && movement.z == 0.0f)
				return;

			auto position = transform.getPosition();
			position += dx3d::Vec3::normalize(movement) * (m_speed * deltaTime);
			transform.setPosition(position);
		}

	private:
		Player& m_player;
		dx3d::Vec3 m_direction{};
		dx3d::f32 m_speed{};
	};

	class LookCommand final : public dx3d::InputCommand
	{
	public:
		LookCommand(Player& player, dx3d::f32 sensitivity) :
			m_player(player),
			m_sensitivity(sensitivity)
		{
		}

		virtual void execute(dx3d::f32) override
		{
			if (ImGui::GetIO().WantCaptureMouse)
				return;

			auto& input = m_player.getInputSystem();
			if (!input.isCursorLocked())
				return;

			auto delta = input.getMouseDelta();
			auto rotation = m_player.getTransform().getRotation();

			rotation.x += delta.y * m_sensitivity;
			rotation.y += delta.x * m_sensitivity;

			if (rotation.x > 1.57f) rotation.x = 1.57f;
			else if (rotation.x < -1.57f) rotation.x = -1.57f;

			m_player.getTransform().setRotation(rotation);
		}

	private:
		Player& m_player;
		dx3d::f32 m_sensitivity{};
	};

	class QuitCommand final : public dx3d::InputCommand
	{
	public:
		void execute(dx3d::f32) override
		{
			PostQuitMessage(0);
		}
	};
}

Player::Player(const dx3d::GameObjectDesc& desc) : dx3d::GameObject(desc)
{
}

Player::~Player()
{
	auto& input = getInputSystem();
	input.unregisterListener(*this);
	input.clearCommands();
}

void Player::onCreate()
{
	auto camera = createOrGetComponent<dx3d::CameraComponent>();
	camera->setNearPlane(0.05f);
	camera->setFarPlane(250.0f);
	camera->setFieldOfView(1.1f);

	auto& input = getInputSystem();
	input.registerListener(*this);

	input.bindCommand(dx3d::KeyCode::W, dx3d::InputTrigger::Held,
		std::make_unique<MoveCommand>(*this, dx3d::Vec3{ 0.0f, 0.0f, 1.0f }, 3.0f));

	input.bindCommand(dx3d::KeyCode::S, dx3d::InputTrigger::Held,
		std::make_unique<MoveCommand>(*this, dx3d::Vec3{ 0.0f, 0.0f, -1.0f }, 3.0f));

	input.bindCommand(dx3d::KeyCode::D, dx3d::InputTrigger::Held,
		std::make_unique<MoveCommand>(*this, dx3d::Vec3{ 1.0f, 0.0f, 0.0f }, 3.0f));

	input.bindCommand(dx3d::KeyCode::A, dx3d::InputTrigger::Held,
		std::make_unique<MoveCommand>(*this, dx3d::Vec3{ -1.0f, 0.0f, 0.0f }, 3.0f));

	input.bindCommand(dx3d::KeyCode::Unknown, dx3d::InputTrigger::MouseMoved,
		std::make_unique<LookCommand>(*this, 0.001f));

	input.bindCommand(dx3d::KeyCode::Q, dx3d::InputTrigger::Pressed,
		std::make_unique<QuitCommand>());
}

void Player::onUpdate(dx3d::f32 deltaTime)
{
	GameObject::onUpdate(deltaTime);
}

void Player::onKeyPressed(dx3d::KeyCode key)
{
	if (key != dx3d::KeyCode::Escape)
		return;

	m_cursorLocked = !m_cursorLocked;
	auto& input = getInputSystem();
	input.setCursorLocked(m_cursorLocked);
	input.setCursorVisible(!m_cursorLocked);
}
