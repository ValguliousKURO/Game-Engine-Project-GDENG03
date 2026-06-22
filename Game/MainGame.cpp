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

#include "MainGame.h"
#include <Windows.h>
#include <cmath>
#include <numbers>

namespace
{
	class SpawnCircleCommand final : public dx3d::InputCommand
	{
	public:
		explicit SpawnCircleCommand(MainGame& game) : m_game(game) {}

		void execute(dx3d::f32) override
		{
			m_game.spawnCircle();
		}

	private:
		MainGame& m_game;
	};

	class RemoveRecentCircleCommand final : public dx3d::InputCommand
	{
	public:
		explicit RemoveRecentCircleCommand(MainGame& game) : m_game(game) {}

		void execute(dx3d::f32) override
		{
			m_game.removeMostRecentCircle();
		}

	private:
		MainGame& m_game;
	};

	class RemoveAllCirclesCommand final : public dx3d::InputCommand
	{
	public:
		explicit RemoveAllCirclesCommand(MainGame& game) : m_game(game) {}

		void execute(dx3d::f32) override
		{
			m_game.removeAllCircles();
		}

	private:
		MainGame& m_game;
	};

	class QuitGameCommand final : public dx3d::InputCommand
	{
	public:
		explicit QuitGameCommand(MainGame& game) : m_game(game) {}

		void execute(dx3d::f32) override
		{
			m_game.quit();
		}

	private:
		MainGame& m_game;
	};
}


MainGame::MainGame(const dx3d::GameDesc& desc) : dx3d::Game(desc)
{
	std::random_device randomDevice{};
	m_randomEngine.seed(randomDevice());
}

MainGame::~MainGame()
{
	auto& input = getInputSystem();
	input.unregisterListener(*this);
	input.clearCommands();
}

void MainGame::onCreate()
{
	Game::onCreate();
	auto& world = getWorld();

	m_camera = world.createGameObject<dx3d::GameObject>();
	auto camera = m_camera->createOrGetComponent<dx3d::CameraComponent>();
	camera->setNearPlane(0.05f);
	camera->setFarPlane(100.0f);
	camera->setFieldOfView(1.1f);
	m_camera->getTransform().setPosition({ 0.0f, 0.0f, -10.0f });

	getInputSystem().setCursorLocked(false);
	getInputSystem().setCursorVisible(true);
	getInputSystem().registerListener(*this);
	getInputSystem().bindCommand(dx3d::KeyCode::Space, dx3d::InputTrigger::Pressed,
		std::make_unique<SpawnCircleCommand>(*this));
	getInputSystem().bindCommand(dx3d::KeyCode::Backspace, dx3d::InputTrigger::Pressed,
		std::make_unique<RemoveRecentCircleCommand>(*this));
	getInputSystem().bindCommand(dx3d::KeyCode::Delete, dx3d::InputTrigger::Pressed,
		std::make_unique<RemoveAllCirclesCommand>(*this));
	getInputSystem().bindCommand(dx3d::KeyCode::Escape, dx3d::InputTrigger::Pressed,
		std::make_unique<QuitGameCommand>(*this));

	spawnCircle();
}


void MainGame::onUpdate(dx3d::f32 deltaTime)
{
	Game::onUpdate(deltaTime);

	constexpr auto horizontalLimit = 7.6f;
	constexpr auto verticalLimit = 5.6f;

	for (auto& circle : m_circles)
	{
		circle.position += circle.velocity * deltaTime;
		circle.angle += circle.angularVelocity * deltaTime;

		if (circle.position.x - circle.radius < -horizontalLimit)
		{
			circle.position.x = -horizontalLimit + circle.radius;
			circle.velocity.x *= -1.0f;
		}
		else if (circle.position.x + circle.radius > horizontalLimit)
		{
			circle.position.x = horizontalLimit - circle.radius;
			circle.velocity.x *= -1.0f;
		}

		if (circle.position.y - circle.radius < -verticalLimit)
		{
			circle.position.y = -verticalLimit + circle.radius;
			circle.velocity.y *= -1.0f;
		}
		else if (circle.position.y + circle.radius > verticalLimit)
		{
			circle.position.y = verticalLimit - circle.radius;
			circle.velocity.y *= -1.0f;
		}

		circle.object->getTransform().setPosition(circle.position);
		circle.object->getTransform().setRotation({ 0.0f, 0.0f, circle.angle });
	}
}

void MainGame::onKeyPressed(dx3d::KeyCode key)
{
	m_lastPressedKey = key;
}

void MainGame::spawnCircle()
{
	constexpr auto pi = std::numbers::pi_v<dx3d::f32>;
	constexpr auto horizontalLimit = 7.6f;
	constexpr auto verticalLimit = 5.6f;

	std::uniform_real_distribution<dx3d::f32> radiusDistribution(0.35f, 0.65f);
	std::uniform_real_distribution<dx3d::f32> angleDistribution(0.0f, pi * 2.0f);
	std::uniform_real_distribution<dx3d::f32> speedDistribution(3.0f, 5.0f);
	std::uniform_real_distribution<dx3d::f32> spinDistribution(-4.0f, 4.0f);

	const auto radius = radiusDistribution(m_randomEngine);
	std::uniform_real_distribution<dx3d::f32> xDistribution(-horizontalLimit + radius, horizontalLimit - radius);
	std::uniform_real_distribution<dx3d::f32> yDistribution(-verticalLimit + radius, verticalLimit - radius);

	const auto directionAngle = angleDistribution(m_randomEngine);
	const auto speed = speedDistribution(m_randomEngine);
	const auto objectAngle = angleDistribution(m_randomEngine);

	auto circleObject = getWorld().createGameObject<dx3d::GameObject>();
	circleObject->createOrGetComponent<dx3d::CircleComponent>();
	circleObject->getTransform().setScale({ radius * 2.0f, radius * 2.0f, radius * 2.0f });
	circleObject->getTransform().setPosition({ xDistribution(m_randomEngine), yDistribution(m_randomEngine), 0.0f });
	circleObject->getTransform().setRotation({ 0.0f, 0.0f, objectAngle });

	m_circles.push_back({
		circleObject,
		circleObject->getTransform().getPosition(),
		{std::cos(directionAngle) * speed, std::sin(directionAngle) * speed, 0.0f},
		radius,
		objectAngle,
		spinDistribution(m_randomEngine)
	});
}

void MainGame::removeMostRecentCircle()
{
	if (m_circles.empty())
	{
		return;
	}

	m_circles.back().object->getTransform().setScale({ 0.0f, 0.0f, 0.0f });
	m_circles.pop_back();
}

void MainGame::removeAllCircles()
{
	for (auto& circle : m_circles)
	{
		circle.object->getTransform().setScale({ 0.0f, 0.0f, 0.0f });
	}

	m_circles.clear();
}

void MainGame::quit()
{
	PostQuitMessage(0);
}
