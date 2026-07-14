
#include "MainGame.h"
#include "Objects/Player.h"
#include <cmath>
#include <imgui.h>
#include <Windows.h>


MainGame::MainGame(const dx3d::GameDesc& desc) : dx3d::Game(desc)
{
}

void MainGame::onCreate()
{
	Game::onCreate();
	auto& world = getWorld();

	m_plane = world.createGameObject<dx3d::GameObject>();
	m_plane->createOrGetComponent<dx3d::PlaneComponent>();
	m_plane->getTransform().setScale({ 14.0f, 1.0f, 14.0f });
	m_plane->getTransform().setPosition({ 0.0f, -1.0f, 0.0f });

	/*m_cubeLeft = world.createGameObject<dx3d::GameObject>();
	m_cubeLeft->createOrGetComponent<dx3d::CubeComponent>();
	m_cubeLeft->getTransform().setScale({ 1.0f, 2.0f, 1.0f });
	m_cubeLeft->getTransform().setPosition({ -3.0f, 0.0f, 4.0f });*/

	m_cubeRight = world.createGameObject<dx3d::GameObject>();
	m_cubeRight->createOrGetComponent<dx3d::CubeComponent>();
	m_cubeRight->getTransform().setScale({ 1.5f, 1.5f, 1.5f });
	m_cubeRight->getTransform().setPosition({ 3.0f, -0.25f, 6.0f });
	m_cubeRight->getTransform().setRotation({ 0.0f, 0.7f, 0.0f });

	/*m_sphere = world.createGameObject<dx3d::GameObject>();
	m_sphere->createOrGetComponent<dx3d::SphereComponent>();
	m_sphere->getTransform().setScale({ 1.4f, 1.4f, 1.4f });
	m_sphere->getTransform().setPosition({ 0.0f, 1.0f, 7.5f });*/

	m_player = world.createGameObject<Player>();
	m_player->getTransform().setPosition({ 0.0f, 1.5f, -7.0f });
	m_player->getTransform().setRotation({ 0.0f, 0.0f, 0.0f });

	getInputSystem().setCursorLocked(true);
	getInputSystem().setCursorVisible(false);
}


void MainGame::onUpdate(dx3d::f32 deltaTime)
{
	Game::onUpdate(deltaTime);

	m_elapsedTime += deltaTime;

	if (m_cubeLeft)
	{
		m_cubeLeft->getTransform().setRotation({ 0.0f, m_elapsedTime, 0.0f });
	}

	if (m_cubeRight)
	{
		m_cubeRight->getTransform().setRotation({ m_elapsedTime * 0.45f, 0.7f, m_elapsedTime * 0.25f });
	}

	if (m_sphere)
	{
		const auto scale = 1.25f + (std::sin(m_elapsedTime * 1.5f) * 0.2f);
		m_sphere->getTransform().setScale({ scale, scale, scale });
	}
}

void MainGame::onRenderUI()
{
	// 1. Menu Bar
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit", "Alt+F4"))
			{
				PostQuitMessage(0);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Windows"))
		{
			ImGui::MenuItem("Scene Hierarchy", nullptr, &m_showHierarchy);
			ImGui::MenuItem("Inspector", nullptr, &m_showInspector);
			ImGui::MenuItem("Credits", nullptr, &m_showCredits);
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	// 2. Credits Window (Formatted exactly like your reference image)
	if (m_showCredits)
	{
		ImGui::Begin("Credits", &m_showCredits, ImGuiWindowFlags_AlwaysAutoResize);
		
		if (ImGui::CollapsingHeader("About", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("Zero Systems Engine");
			ImGui::Text("By Arvin Dacanay");
			ImGui::Text("Conversation is the cornerstone to evolution.");
			ImGui::Text("IDK what to put here...");
		}
		
		ImGui::End();
	}

	// 3. Scene Hierarchy Window
	if (m_showHierarchy)
	{
		ImGui::Begin("Scene Hierarchy", &m_showHierarchy);
		
		auto drawSelectable = [this](const char* label, dx3d::GameObject* obj) {
			bool selected = (m_selectedObject == obj);
			if (ImGui::Selectable(label, selected))
			{
				m_selectedObject = obj;
			}
		};

		drawSelectable("Grid/Plane", m_plane);
		drawSelectable("Cube (Right)", m_cubeRight);
		drawSelectable("Player / Main Camera", m_player);

		ImGui::End();
	}

	// 4. Inspector Window
	if (m_showInspector)
	{
		ImGui::Begin("Inspector", &m_showInspector);
		if (m_selectedObject)
		{
			// Show name based on object type
			const char* name = "Unknown GameObject";
			if (m_selectedObject == m_plane) name = "Grid/Plane";
			else if (m_selectedObject == m_cubeRight) name = "Cube (Right)";
			else if (m_selectedObject == m_player) name = "Player / Main Camera";

			ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.4f, 1.0f), "Name: %s", name);
			ImGui::Separator();

			auto& transform = m_selectedObject->getTransform();
			
			// Position
			dx3d::Vec3 pos = transform.getPosition();
			float p[3] = { pos.x, pos.y, pos.z };
			if (ImGui::DragFloat3("Position", p, 0.05f))
			{
				transform.setPosition({ p[0], p[1], p[2] });
			}

			// Rotation
			dx3d::Vec3 rot = transform.getRotation();
			float r[3] = { rot.x, rot.y, rot.z };
			if (ImGui::DragFloat3("Rotation", r, 0.05f))
			{
				transform.setRotation({ r[0], r[1], r[2] });
			}

			// Scale
			dx3d::Vec3 scale = transform.getScale();
			float s[3] = { scale.x, scale.y, scale.z };
			if (ImGui::DragFloat3("Scale", s, 0.05f))
			{
				transform.setScale({ s[0], s[1], s[2] });
			}

			ImGui::Separator();
			
			// Show controls info
			ImGui::TextWrapped("Click & drag values to modify the transform in real time.");
		}
		else
		{
			ImGui::Text("Select a GameObject from the Scene Hierarchy to inspect its properties.");
		}
		ImGui::End();
	}
}
