

#pragma once
#include <DX3D/All.h>
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
	void quit();
protected:
	virtual void onCreate();
	virtual void onUpdate(dx3d::f32 deltaTime);
	virtual void onKeyPressed(dx3d::KeyCode key) override;


private:
	struct Circle
	{
		dx3d::GameObject* object{};
		dx3d::Vec3 position{};
		dx3d::Vec3 velocity{};
		dx3d::f32 radius{};
		dx3d::f32 angle{};
		dx3d::f32 angularVelocity{};
	};

private:
	dx3d::GameObject* m_camera{};
	std::vector<Circle> m_circles{};
	std::mt19937 m_randomEngine{};
	dx3d::KeyCode m_lastPressedKey{ dx3d::KeyCode::Unknown };
};

