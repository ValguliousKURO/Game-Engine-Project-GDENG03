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
#include "Objects/Player.h"
#include <cmath>
#include <random>


MainGame::MainGame(const dx3d::GameDesc& desc) : dx3d::Game(desc)
{
}

void MainGame::onCreate()
{
	Game::onCreate();
	auto& world = getWorld();


	//m_plane = world.createGameObject<dx3d::GameObject>();
	//m_plane->createOrGetComponent<dx3d::PlaneComponent>();
	//m_plane->getTransform().setPosition({ -0.75f, 0.0f, -1.0f });
	//m_plane->getTransform().setScale({ 20.0f, 1.0f, 20.0f });

	//m_cube1 = world.createGameObject<dx3d::GameObject>();
	//m_cube1->createOrGetComponent<dx3d::CubeComponent>();
	//m_cube1->getTransform().setPosition({ 0.0f, 0.9f, 0.0f });
	//m_cube1->getTransform().setScale({ 1.0f, 1.0f, 1.0f });
	//m_cube1->getTransform().setRotation({ 0.0f, 0.0f, 0.0f });

	//m_cube2 = world.createGameObject<dx3d::GameObject>();
	//m_cube2->createOrGetComponent<dx3d::CubeComponent>();
	//m_cube2->getTransform().setPosition({ -1.5f, 2.0f, 0.0f });
	//m_cube2->getTransform().setScale({ 1.0f, 1.0f, 1.0f });
	//m_cube2->getTransform().setRotation({ 0.0f, 0.0f, 0.0f });

	m_cube3 = world.createGameObject<dx3d::GameObject>();
	m_cube3->createOrGetComponent<dx3d::CubeComponent>();
	m_cube3->getTransform().setPosition({ -1.5f, 3.0f, -2.0f });
	m_cube3->getTransform().setScale({ 1.0f, 1.0f, 1.0f });
	m_cube3->getTransform().setRotation({ 0.0f, 0.0f, 0.0f });

	m_player = world.createGameObject<Player>();
	m_player->getTransform().setPosition({ 4.0f, 5.0f, 5.0f });
	m_player->getTransform().setRotation({ 0.374f, -2.472f, 0.0f });


	/*m_plane = world.createGameObject<dx3d::GameObject>();
	m_plane->createOrGetComponent<dx3d::PlaneComponent>();
	m_plane->getTransform().setPosition({ 1.0f, -1.5f, 10.0f });
	m_plane->getTransform().setScale({ 10.0f, 1.0f, 10.0f });
	
	m_player = world.createGameObject<Player>();
	m_player->getTransform().setPosition({ 0.0f, 1.5f, -7.0f });
	m_player->getTransform().setRotation({ 0.0f, 0.0f, 0.0f });*/

	//std::mt19937 generator(1337); // Seed for reproducible/consistent random positions
	//std::uniform_real_distribution<float> posXDist(-15.0f, 15.0f);
	//std::uniform_real_distribution<float> posYDist(-1.0f, 10.0f);
	//std::uniform_real_distribution<float> posZDist(-5.0f, 25.0f);

	//std::uniform_real_distribution<float> rotDist(0.0f, 6.28f);
	//std::uniform_real_distribution<float> rotSpeedDist(-1.5f, 1.5f);
	//std::uniform_real_distribution<float> scaleDist(0.5f, 1.5f);

	//for (int i = 0; i < 50; ++i)
	//{
	//	CubeData cube;
	//	cube.gameObject = world.createGameObject<dx3d::GameObject>();
	//	cube.gameObject->createOrGetComponent<dx3d::CubeComponent>();

	//	// random transform values
	//	cube.gameObject->getTransform().setPosition({ posXDist(generator), posYDist(generator), posZDist(generator) });
	//	cube.gameObject->getTransform().setRotation({ rotDist(generator), rotDist(generator), rotDist(generator) });
	//	
	//	float s = scaleDist(generator);
	//	cube.gameObject->getTransform().setScale({ s, s, s });

	//	// random rotation speeds 
	//	float rx = rotSpeedDist(generator);
	//	float ry = rotSpeedDist(generator);
	//	float rz = rotSpeedDist(generator);
	//	if (std::abs(rx) < 0.2f) rx = rx < 0 ? -0.2f : 0.2f;
	//	if (std::abs(ry) < 0.2f) ry = ry < 0 ? -0.2f : 0.2f;
	//	if (std::abs(rz) < 0.2f) rz = rz < 0 ? -0.2f : 0.2f;

	//	cube.rotationSpeed = { rx, ry, rz };

	//	m_cubes.push_back(cube);
	//}

	getInputSystem().setCursorLocked(true);
	getInputSystem().setCursorVisible(false);
}


void MainGame::onUpdate(dx3d::f32 deltaTime)
{
	Game::onUpdate(deltaTime);

	/*m_elapsedTime += deltaTime;

	for (auto& cube : m_cubes)
	{
		if (cube.gameObject)
		{
			auto rotation = cube.gameObject->getTransform().getRotation();
			rotation.x += cube.rotationSpeed.x * deltaTime;
			rotation.y += cube.rotationSpeed.y * deltaTime;
			rotation.z += cube.rotationSpeed.z * deltaTime;
			cube.gameObject->getTransform().setRotation(rotation);
		}
	}*/
}
