

#pragma once
#include <DX3D/All.h>
#include <vector>


class MainGame : public dx3d::Game
{
public:
	explicit MainGame(const dx3d::GameDesc& desc);
protected:
	virtual void onCreate();
	virtual void onUpdate(dx3d::f32 deltaTime);


private:
	dx3d::GameObject* m_player{};
	// dx3d::GameObject* m_warpingCube{};
	// dx3d::GameObject* m_cube1{};
	// dx3d::GameObject* m_cube2{};
	// dx3d::GameObject* m_cube3{};
	// dx3d::GameObject* m_plane{};
	std::vector<dx3d::GameObject*> m_cards;
	dx3d::f32 m_elapsedTime{};
};

