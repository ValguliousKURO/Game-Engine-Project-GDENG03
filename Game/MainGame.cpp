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

	class UndoCommand final : public dx3d::InputCommand
	{
	public:
		explicit UndoCommand(MainGame& game) : m_game(game) {}

	void execute(dx3d::f32) override
	{
		m_game.undo();
	}

	private:
		MainGame& m_game;
	};

	class RedoCommand final : public dx3d::InputCommand
	{
	public:
		explicit RedoCommand(MainGame& game) : m_game(game) {}

	void execute(dx3d::f32) override
	{
		m_game.redo();
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

class MainGame::CircleEditCommand
{
public:
	virtual ~CircleEditCommand() = default;
	virtual bool execute(MainGame& game) = 0;
	virtual void undo(MainGame& game) = 0;
};

namespace
{
	class SpawnCircleEditCommand final : public MainGame::CircleEditCommand
	{
	public:
		bool execute(MainGame& game) override
		{
			if (!m_initialized)
			{
				m_state = game.createRandomCircleState();
				m_initialized = true;
			}

			m_circleIndex = game.addCircle(m_state);
			return true;
		}

		void undo(MainGame& game) override
		{
			game.deactivateCircle(m_circleIndex);
		}

	private:
		MainGame::CircleState m_state{};
		size_t m_circleIndex{};
		bool m_initialized{};
	};

	class RemoveRecentCircleEditCommand final : public MainGame::CircleEditCommand
	{
	public:
		bool execute(MainGame& game) override
		{
			if (!game.findMostRecentActiveCircle(m_circleIndex))
			{
				return false;
			}

			game.getCircleState(m_circleIndex, m_state);
			game.deactivateCircle(m_circleIndex);
			return true;
		}

		void undo(MainGame& game) override
		{
			game.activateCircle(m_circleIndex, m_state);
		}

	private:
		MainGame::CircleState m_state{};
		size_t m_circleIndex{};
	};

	class RemoveAllCirclesEditCommand final : public MainGame::CircleEditCommand
	{
	public:
		bool execute(MainGame& game) override
		{
			m_removedCircles.clear();

			for (auto index = size_t{ 0 }; index < game.getCircleCount(); ++index)
			{
				if (!game.isCircleActive(index))
				{
					continue;
				}

				MainGame::CircleState state{};
				game.getCircleState(index, state);
				m_removedCircles.push_back({ index, state });
				game.deactivateCircle(index);
			}

			return !m_removedCircles.empty();
		}

		void undo(MainGame& game) override
		{
			for (const auto& removedCircle : m_removedCircles)
			{
				game.activateCircle(removedCircle.index, removedCircle.state);
			}
		}

	private:
		struct RemovedCircle
		{
			size_t index{};
			MainGame::CircleState state{};
		};

		std::vector<RemovedCircle> m_removedCircles{};
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

	auto& input = getInputSystem();
	input.setCursorLocked(false);
	input.setCursorVisible(true);
	input.registerListener(*this);
	input.bindCommand(dx3d::KeyCode::Space, dx3d::InputTrigger::Pressed,
		std::make_unique<SpawnCircleCommand>(*this));
	input.bindCommand(dx3d::KeyCode::Backspace, dx3d::InputTrigger::Pressed,
		std::make_unique<RemoveRecentCircleCommand>(*this));
	input.bindCommand(dx3d::KeyCode::Delete, dx3d::InputTrigger::Pressed,
		std::make_unique<RemoveAllCirclesCommand>(*this));
	input.bindCommand(dx3d::KeyCode::Z, dx3d::InputTrigger::Pressed,
		std::make_unique<UndoCommand>(*this));
	input.bindCommand(dx3d::KeyCode::Y, dx3d::InputTrigger::Pressed,
		std::make_unique<RedoCommand>(*this));
	input.bindCommand(dx3d::KeyCode::Escape, dx3d::InputTrigger::Pressed,
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
		if (!circle.active)
		{
			continue;
		}

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

		applyCircleTransform(circle);
	}
}

void MainGame::onKeyPressed(dx3d::KeyCode key)
{
	m_lastPressedKey = key;
}

void MainGame::spawnCircle()
{
	executeCircleCommand(std::make_unique<SpawnCircleEditCommand>());
}

void MainGame::removeMostRecentCircle()
{
	executeCircleCommand(std::make_unique<RemoveRecentCircleEditCommand>());
}

void MainGame::removeAllCircles()
{
	executeCircleCommand(std::make_unique<RemoveAllCirclesEditCommand>());
}

void MainGame::undo()
{
	if (m_undoCommands.empty())
	{
		return;
	}

	auto command = std::move(m_undoCommands.back());
	m_undoCommands.pop_back();
	command->undo(*this);
	m_redoCommands.push_back(std::move(command));
}

void MainGame::redo()
{
	if (m_redoCommands.empty())
	{
		return;
	}

	auto command = std::move(m_redoCommands.back());
	m_redoCommands.pop_back();
	if (command->execute(*this))
	{
		m_undoCommands.push_back(std::move(command));
	}
}

void MainGame::quit()
{
	PostQuitMessage(0);
}

MainGame::CircleState MainGame::createRandomCircleState()
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

	return {
		{xDistribution(m_randomEngine), yDistribution(m_randomEngine), 0.0f},
		{std::cos(directionAngle) * speed, std::sin(directionAngle) * speed, 0.0f},
		radius,
		angleDistribution(m_randomEngine),
		spinDistribution(m_randomEngine)
	};
}

size_t MainGame::addCircle(const CircleState& state)
{
	auto circleObject = getWorld().createGameObject<dx3d::GameObject>();
	circleObject->createOrGetComponent<dx3d::SphereComponent>();  //3d sphere = submitted
	//circleObject->createOrGetComponent<dx3d::CircleComponent>(); //2d circle

	m_circles.push_back({
		circleObject,
		state.position,
		state.velocity,
		state.radius,
		state.angle,
		state.angularVelocity,
		true
		});

	applyCircleTransform(m_circles.back());
	return m_circles.size() - 1u;
}

void MainGame::activateCircle(size_t index, const CircleState& state)
{
	if (index >= m_circles.size())
	{
		return;
	}

	auto& circle = m_circles[index];
	circle.position = state.position;
	circle.velocity = state.velocity;
	circle.radius = state.radius;
	circle.angle = state.angle;
	circle.angularVelocity = state.angularVelocity;
	circle.active = true;
	applyCircleTransform(circle);
}

void MainGame::deactivateCircle(size_t index)
{
	if (index >= m_circles.size())
	{
		return;
	}

	auto& circle = m_circles[index];
	circle.active = false;
	if (circle.object)
	{
		circle.object->getTransform().setScale({ 0.0f, 0.0f, 0.0f });
	}
}

bool MainGame::findMostRecentActiveCircle(size_t& index) const
{
	for (auto i = m_circles.size(); i > 0u; --i)
	{
		const auto candidate = i - 1u;
		if (m_circles[candidate].active)
		{
			index = candidate;
			return true;
		}
	}

	return false;
}

bool MainGame::getCircleState(size_t index, CircleState& state) const
{
	if (index >= m_circles.size())
	{
		return false;
	}

	const auto& circle = m_circles[index];
	state = { circle.position, circle.velocity, circle.radius, circle.angle, circle.angularVelocity };
	return true;
}

bool MainGame::isCircleActive(size_t index) const
{
	return index < m_circles.size() && m_circles[index].active;
}

size_t MainGame::getCircleCount() const
{
	return m_circles.size();
}

void MainGame::executeCircleCommand(std::unique_ptr<CircleEditCommand> command)
{
	if (!command)
	{
		return;
	}

	if (!command->execute(*this))
	{
		return;
	}

	m_undoCommands.push_back(std::move(command));
	m_redoCommands.clear();
}

void MainGame::applyCircleTransform(Circle& circle)
{
	if (!circle.object)
	{
		return;
	}

	circle.object->getTransform().setScale({ circle.radius * 2.0f, circle.radius * 2.0f, circle.radius * 2.0f });
	circle.object->getTransform().setPosition(circle.position);
	circle.object->getTransform().setRotation({ 0.0f, 0.0f, circle.angle });
}
