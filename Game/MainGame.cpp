

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

	//m_cube3 = world.createGameObject<dx3d::GameObject>();
	//m_cube3->createOrGetComponent<dx3d::CubeComponent>();
	//m_cube3->getTransform().setPosition({ -1.5f, 3.0f, -2.0f });
	//m_cube3->getTransform().setScale({ 1.0f, 1.0f, 1.0f });
	//m_cube3->getTransform().setRotation({ 0.0f, 0.0f, 0.0f });

	//m_player = world.createGameObject<Player>();
	//m_player->getTransform().setPosition({ 4.0f, 5.0f, 5.0f });
	//m_player->getTransform().setRotation({ 0.374f, -2.472f, 0.0f });

	//m_warpingCube = world.createGameObject<dx3d::GameObject>();
	//m_warpingCube->createOrGetComponent<dx3d::CubeComponent>();
	//m_warpingCube->getTransform().setPosition({ 0.0f, 0.0f, 0.0f });
	//m_warpingCube->getTransform().setScale({ 1.0f, 1.0f, 1.0f });
	//m_warpingCube->getTransform().setRotation({ 0.0f, 0.0f, 0.0f });

	//m_player = world.createGameObject<Player>();
	//m_player->getTransform().setPosition({ 0.0f, 2.0f, -6.0f });
	//m_player->getTransform().setRotation({ 0.32f, 0.0f, 0.0f });

	// Stacking Cards
	auto createCard = [&world, this](const dx3d::Vec3& position, const dx3d::Vec3& scale, const dx3d::Vec3& rotation) {
		auto card = world.createGameObject<dx3d::GameObject>();
		card->createOrGetComponent<dx3d::CubeComponent>();
		card->getTransform().setPosition(position);
		card->getTransform().setScale(scale);
		card->getTransform().setRotation(rotation);
		m_cards.push_back(card);
	};

	// Bottom layer
	// Tent 1 (Leftmost): Center X = -1.4f
	createCard({ -1.616f, 0.666f, 0.0f }, { 1.0f, 1.4f, 0.02f }, { 0.0f, 1.57f, -0.31f }); // Card 1: Left leg (leans right)
	createCard({ -1.184f, 0.666f, 0.0f }, { 1.0f, 1.4f, 0.02f }, {0.0f, 1.57f, 0.31f });  // Card 2: Right leg (leans left)

	// Tent 2 (Middle): Center X = 0.0f
	createCard({ -0.216f, 0.666f, 0.0f }, { 1.0f, 1.4f, 0.02f }, { 0.0f, 1.57f, -0.31f }); // Card 3: Left leg
	createCard({ 0.216f, 0.666f, 0.0f }, { 1.0f, 1.4f, 0.02f }, {0.0f, 1.57f, 0.31f });  // Card 4: Right leg
	// Tent 3 (Rightmost): Center X = 1.4f
	createCard({ 1.184f, 0.666f, 0.0f }, { 1.0f, 1.4f, 0.02f }, { 0.0f, 1.5708f, -0.31f }); // Card 5: Left leg
	createCard({ 1.616f, 0.666f, 0.0f }, { 1.0f, 1.4f, 0.02f }, {0.0f, 1.5708f, 0.31f });  // Card 6: Right leg

	// Horizontal Connectors (Bottom layer):
	createCard({ -0.7f, 1.33f, 0.0f }, { 1.4f, 0.02f, 1.0f }, { 0.0f, 0.0f, 0.0f });       // Card 7: Left flat card
	createCard({ 0.7f, 1.33f, 0.0f }, { 1.4f, 0.02f, 1.0f }, { 0.0f, 0.0f, 0.0f });        // Card 8: Right flat card

	// middle layer
	// Tent 4 (Left middle): Center X = -0.7f
	createCard({ -0.916f, 1.996f, 0.0f }, { 1.0f, 1.4f, 0.02f }, { 0.0f, 1.57f, -0.31f });// Card 9: Left leg
	createCard({ -0.484f, 1.996f, 0.0f }, { 1.0f, 1.4f, 0.02f }, { 0.0f, 1.57f, 0.31f }); // Card 10: Right leg

	// Tent 5 (Right middle): Center X = 0.7f
	createCard({ 0.484f, 1.996f, 0.0f }, { 1.0f, 1.4f, 0.02f }, { 0.0f, 1.57f, -0.31f }); // Card 11: Left leg
	createCard({ 0.916f, 1.996f, 0.0f }, { 1.0f, 1.4f, 0.02f }, { 0.0f, 1.57f, 0.31f });  // Card 12: Right leg
	// Horizontal Connector (Middle layer):
	createCard({ 0.0f, 2.66f, 0.0f }, { 1.4f, 0.02f, 1.0f }, { 0.0f, 0.0f, 0.0f });        // Card 13: Middle flat card

	// TOP layer
	// Tent 6 (Top): Center X = 0.0f
	createCard({ -0.216f, 3.326f, 0.0f }, { 1.0f, 1.4f, 0.02f }, { 0.0f, 1.57f, -0.31f });// Card 14: Left leg
	createCard({ 0.216f, 3.326f, 0.0f }, { 1.0f, 1.4f, 0.02f }, { 0.0f, 1.57f, 0.31f });  // Card 15: Right leg

	// Camera setup - moved far away to remove perspective distortion (making it look isometric like the reference picture)
	m_player = world.createGameObject<Player>();
	m_player->getTransform().setPosition({ 5.0f, 10.0f, 12.0f });
	m_player->getTransform().setRotation({ 0.400f, -2.45f, 0.0f });

	auto camera = m_player->getComponent<dx3d::CameraComponent>();
	if (camera)
	{
		camera->setFieldOfView(0.275f);
	}

	getInputSystem().setCursorLocked(true);
	getInputSystem().setCursorVisible(false);
}


void MainGame::onUpdate(dx3d::f32 deltaTime)
{
	Game::onUpdate(deltaTime);

	m_elapsedTime += deltaTime;

	/*if (m_warpingCube)
	{
		// t goes between 0.0f (uniform cube) and 1.0f (flat horizontal plane)
		float t = (std::sin(m_elapsedTime * 1.5f) + 1.0f) * 0.5f;

		// We scale the local X and Y dimensions to 6.0f (larger scale) and flatten the Z dimension to 0.01f.
		// By rotating the cube 90 degrees (1.5708 rad) around the X-axis, the flat plane is aligned
		// horizontally in world coordinates, satisfying both "horizontal plane" and "larger X, Y scale".
		float scaleX = 1.0f + 5.0f * t;
		float scaleY = 1.0f + 5.0f * t;
		float scaleZ = 1.0f - 0.99f * t;
		float rotationX = 1.5708f * t;

		m_warpingCube->getTransform().setScale({ scaleX, scaleY, scaleZ });
		m_warpingCube->getTransform().setRotation({ rotationX, 0.0f, 0.0f });
	}*/

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
