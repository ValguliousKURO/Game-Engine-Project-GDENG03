#pragma once
#include <DX3D/All.h>
#include <memory>
#include <random>
#include <vector>

class MainGame : public dx3d::Game, public dx3d::InputListener
{
public:
	explicit MainGame(const dx3d::GameDesc& desc);
	virtual ~MainGame() override;

	void spawnCircle();
	void removeMostRecentCircle();
	void removeAllCircles();
	void undo();
	void redo();
	void quit();

	struct Circle
	{
		dx3d::GameObject* object{};
		dx3d::Vec3 position{};
		dx3d::Vec3 velocity{};
		dx3d::f32 radius{};
		dx3d::f32 angle{};
		dx3d::f32 angularVelocity{};
		bool active{ true };
	};

	struct CircleState
	{
		dx3d::Vec3 position{};
		dx3d::Vec3 velocity{};
		dx3d::f32 radius{};
		dx3d::f32 angle{};
		dx3d::f32 angularVelocity{};
	};

	class CircleEditCommand;

	CircleState createRandomCircleState();
	size_t addCircle(const CircleState& state);
	void activateCircle(size_t index, const CircleState& state);
	void deactivateCircle(size_t index);
	bool findMostRecentActiveCircle(size_t& index) const;
	bool getCircleState(size_t index, CircleState& state) const;
	bool isCircleActive(size_t index) const;
	size_t getCircleCount() const;

protected:
	virtual void onCreate() override;
	virtual void onUpdate(dx3d::f32 deltaTime) override;
	virtual void onKeyPressed(dx3d::KeyCode key) override;

private:
	void executeCircleCommand(std::unique_ptr<CircleEditCommand> command);
	void applyCircleTransform(Circle& circle);

private:
	dx3d::GameObject* m_camera{};
	std::vector<Circle> m_circles{};
	std::vector<std::unique_ptr<CircleEditCommand>> m_undoCommands{};
	std::vector<std::unique_ptr<CircleEditCommand>> m_redoCommands{};
	std::mt19937 m_randomEngine{};
	dx3d::KeyCode m_lastPressedKey{ dx3d::KeyCode::Unknown };
};
