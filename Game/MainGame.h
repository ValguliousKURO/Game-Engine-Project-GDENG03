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

#pragma once
#include <DX3D/All.h>
#include <vector>

struct ID3D11ShaderResourceView;

class MainGame : public dx3d::Game
{
public:
	explicit MainGame(const dx3d::GameDesc& desc);
	virtual ~MainGame() override;
protected:
	virtual void onCreate() override;
	virtual void onUpdate(dx3d::f32 deltaTime) override;
	virtual void onRenderUI() override;


private:
	void createPhysicsCubeStack(dx3d::World& world);
	void resetPhysicsScene();
	void setPlayMode(bool isPlaying);
	void syncPhysicsBodiesFromScene();

	struct CubeSpawnTransform
	{
		dx3d::Vec3 position{};
		dx3d::Vec3 rotation{};
	};

	dx3d::GameObject* m_player{};
	dx3d::GameObject* m_plane{};
	dx3d::GameObject* m_cubeLeft{};
	dx3d::GameObject* m_cubeRight{};
	dx3d::GameObject* m_sphere{};
	dx3d::GameObject* m_teapot{};
	dx3d::GameObject* m_bunny{};
	dx3d::GameObject* m_armadillo{};
	std::vector<dx3d::GameObject*> m_physicsCubes{};
	std::vector<CubeSpawnTransform> m_cubeSpawnTransforms{};
	dx3d::f32 m_elapsedTime{};

	dx3d::GameObject* m_selectedObject{};
	bool m_showCredits{ true };
	bool m_showHierarchy{ true };
	bool m_showInspector{ true };
	bool m_showColorPicker{ false };
	bool m_isPlaying{ false };

	ID3D11ShaderResourceView* m_logoTextureSRV{};
	int m_logoWidth{ 0 };
	int m_logoHeight{ 0 };
	bool m_logoLoaded{ false };
};

