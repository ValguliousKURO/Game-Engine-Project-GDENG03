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


MainGame::MainGame(const dx3d::GameDesc& desc) : dx3d::Game(desc)
{
}

void MainGame::onCreate()
{
	Game::onCreate();
	auto& world = getWorld();

	m_plane = world.createGameObject<dx3d::GameObject>();
	m_plane->createOrGetComponent<dx3d::CubeComponent>();
	m_plane->getTransform().setScale({ 10.0f, 0.1f, 10.0f });
	m_plane->getTransform().setPosition({ 0.0f, -1.0f, 0.0f });

	m_cubeLeft = world.createGameObject<dx3d::GameObject>();
	m_cubeLeft->createOrGetComponent<dx3d::CubeComponent>();
	m_cubeLeft->getTransform().setScale({ 1.0f, 1.0f, 1.0f });
	m_cubeLeft->getTransform().setPosition({ -2.0f, 0.0f, 3.0f });

	m_cubeRight = world.createGameObject<dx3d::GameObject>();
	m_cubeRight->createOrGetComponent<dx3d::CubeComponent>();
	m_cubeRight->getTransform().setScale({ 1.0f, 1.0f, 1.0f });
	m_cubeRight->getTransform().setPosition({ 2.0f, 0.0f, 3.0f });

	m_sphere = world.createGameObject<dx3d::GameObject>();
	m_sphere->createOrGetComponent<dx3d::CubeComponent>();
	m_sphere->getTransform().setScale({ 1.0f, 1.0f, 1.0f });
	m_sphere->getTransform().setPosition({ 0.0f, 1.0f, 6.0f });

	m_player = world.createGameObject<Player>();
	m_player->getTransform().setPosition({ 0.0f, 1.5f, -7.0f });
	m_player->getTransform().setRotation({ 0.0f, 0.0f, 0.0f });

	getInputSystem().setCursorLocked(true);
	getInputSystem().setCursorVisible(false);
}


void MainGame::onUpdate(dx3d::f32 deltaTime)
{
	Game::onUpdate(deltaTime);
}
