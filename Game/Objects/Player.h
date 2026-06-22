

#pragma once
#include <DX3D/All.h>


class Player : public dx3d::GameObject, public dx3d::InputListener
{
	dx3d_typeid(Player)
public:
	explicit Player(const dx3d::GameObjectDesc& desc);
	virtual ~Player() override;
protected:
	virtual void onCreate();
	virtual void onUpdate(dx3d::f32 deltaTime);
	virtual void onKeyPressed(dx3d::KeyCode key) override;

private:
	bool m_cursorLocked{ true };
};

