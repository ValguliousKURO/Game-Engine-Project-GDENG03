#include "MainGame.h"
#include "Objects/Player.h"
#include <cmath>
#include <imgui.h>
#include <Windows.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <d3d11.h>
#include <DX3D/Graphics/GraphicsDevice.h>


MainGame::MainGame(const dx3d::GameDesc& desc) : dx3d::Game(desc)
{
}

// Simple helper function to load a Win32/DirectX11 texture from a file using stb_image
bool LoadTextureFromFile(const char* filename, ID3D11Device* d3dDevice, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height)
{
	// Load from disk into a raw RGBA buffer
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
	if (image_data == NULL)
		return false;

	// Create texture
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = image_width;
	desc.Height = image_height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;

	ID3D11Texture2D* pTexture = NULL;
	D3D11_SUBRESOURCE_DATA subResource;
	subResource.pSysMem = image_data;
	subResource.SysMemPitch = desc.Width * 4;
	subResource.SysMemSlicePitch = 0;
	HRESULT hr = d3dDevice->CreateTexture2D(&desc, &subResource, &pTexture);
	if (FAILED(hr))
	{
		stbi_image_free(image_data);
		return false;
	}

	// Create Shader Resource View
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	hr = d3dDevice->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
	pTexture->Release();

	stbi_image_free(image_data);

	if (FAILED(hr))
		return false;

	*out_width = image_width;
	*out_height = image_height;

	return true;
}

MainGame::~MainGame()
{
	if (m_logoTextureSRV)
	{
		m_logoTextureSRV->Release();
		m_logoTextureSRV = nullptr;
	}
}

void MainGame::onCreate()
{
	Game::onCreate();
	auto& world = getWorld();

	m_plane = world.createGameObject<dx3d::GameObject>();
	m_plane->createOrGetComponent<dx3d::PlaneComponent>();
	m_plane->getTransform().setScale({ 16.0f, 1.0f, 16.0f });
	m_plane->getTransform().setPosition({ 0.0f, -1.0f, 0.0f });
	m_plane->createOrGetComponent<dx3d::RigidBodyComponent>()->configureBox({ 8.0f, 0.15f, 8.0f }, true, 0.0f, 0.25f, 0.85f);

	/*m_cubeLeft = world.createGameObject<dx3d::GameObject>();
	m_cubeLeft->createOrGetComponent<dx3d::CubeComponent>();
	m_cubeLeft->getTransform().setScale({ 1.0f, 2.0f, 1.0f });
	m_cubeLeft->getTransform().setPosition({ -3.0f, 0.0f, 4.0f });*/

	createPhysicsCubeStack(world);

	/*m_sphere = world.createGameObject<dx3d::GameObject>();
	m_sphere->createOrGetComponent<dx3d::SphereComponent>();
	m_sphere->getTransform().setScale({ 1.4f, 1.4f, 1.4f });
	m_sphere->getTransform().setPosition({ 0.0f, 1.0f, 7.5f });*/

	m_player = world.createGameObject<Player>();
	m_player->getTransform().setPosition({ 0.0f, 3.2f, -10.0f });
	m_player->getTransform().setRotation({ 0.22f, 0.0f, 0.0f });

	getInputSystem().setCursorLocked(true);
	getInputSystem().setCursorVisible(false);

	// Load the logo texture
	m_logoLoaded = LoadTextureFromFile("logo.png", getGraphicsDevice().getD3DDevice(), &m_logoTextureSRV, &m_logoWidth, &m_logoHeight);
	if (!m_logoLoaded)
	{
		m_logoLoaded = LoadTextureFromFile("dlsu_logo.png", getGraphicsDevice().getD3DDevice(), &m_logoTextureSRV, &m_logoWidth, &m_logoHeight);
	}
}

void MainGame::createPhysicsCubeStack(dx3d::World& world)
{
	constexpr auto cubeCount = 24;
	const dx3d::Vec3 cubeScale{ 0.65f, 0.65f, 0.65f };
	const dx3d::Vec3 halfExtents{ cubeScale.x * 0.5f, cubeScale.y * 0.5f, cubeScale.z * 0.5f };

	m_physicsCubes.reserve(cubeCount);
	m_cubeSpawnTransforms.reserve(cubeCount);
	for (auto i = 0; i < cubeCount; ++i)
	{
		auto cube = world.createGameObject<dx3d::GameObject>();
		cube->createOrGetComponent<dx3d::CubeComponent>();
		cube->getTransform().setScale(cubeScale);

		const auto column = i % 6;
		const auto row = (i / 6) % 4;
		const auto layer = i / 8;
		const auto x = -2.0f + (static_cast<dx3d::f32>(column) * 0.8f);
		const auto z = 4.0f + (static_cast<dx3d::f32>(row) * 0.8f);
		const auto y = 5.0f + (static_cast<dx3d::f32>(layer) * 0.85f) + (static_cast<dx3d::f32>(column % 2) * 0.25f);

		CubeSpawnTransform spawnTransform{};
		spawnTransform.position = { x, y, z };
		spawnTransform.rotation = {
			0.15f * static_cast<dx3d::f32>(i % 5),
			0.23f * static_cast<dx3d::f32>(i % 7),
			0.31f * static_cast<dx3d::f32>(i % 3)
		};

		cube->getTransform().setPosition(spawnTransform.position);
		cube->getTransform().setRotation(spawnTransform.rotation);
		cube->createOrGetComponent<dx3d::RigidBodyComponent>()->configureBox(halfExtents, false, 1.0f, 0.28f, 0.7f);
		m_physicsCubes.push_back(cube);
		m_cubeSpawnTransforms.push_back(spawnTransform);
	}
}

void MainGame::resetPhysicsScene()
{
	setPlayMode(false);

	for (auto i = 0u; i < m_physicsCubes.size() && i < m_cubeSpawnTransforms.size(); ++i)
	{
		auto cube = m_physicsCubes[i];
		const auto& spawnTransform = m_cubeSpawnTransforms[i];

		cube->getTransform().setPosition(spawnTransform.position);
		cube->getTransform().setRotation(spawnTransform.rotation);

		if (auto rigidBody = cube->getComponent<dx3d::RigidBodyComponent>())
		{
			rigidBody->resetTransform(spawnTransform.position, spawnTransform.rotation);
		}
	}

	m_selectedObject = {};
}

void MainGame::setPlayMode(bool isPlaying)
{
	if (m_isPlaying == isPlaying)
		return;

	m_isPlaying = isPlaying;
	if (m_isPlaying)
	{
		syncPhysicsBodiesFromScene();
	}
	getWorld().setPhysicsEnabled(m_isPlaying);
}

void MainGame::syncPhysicsBodiesFromScene()
{
	for (auto cube : m_physicsCubes)
	{
		auto rigidBody = cube->getComponent<dx3d::RigidBodyComponent>();
		if (!rigidBody)
			continue;

		auto& transform = cube->getTransform();
		rigidBody->resetTransform(transform.getPosition(), transform.getRotation());
	}
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
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Tools"))
		{
			ImGui::MenuItem("Color Picker", nullptr, &m_showColorPicker);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("About"))
		{
			ImGui::MenuItem("Credits", nullptr, &m_showCredits);
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	// 2. Credits Window 
	if (m_showCredits)
	{
		ImGui::Begin("Credits", &m_showCredits, ImGuiWindowFlags_AlwaysAutoResize);
		
		if (m_logoLoaded && m_logoTextureSRV)
		{
			// Render the loaded logo image scaled to 150x150
			ImGui::Image((void*)m_logoTextureSRV, ImVec2(150, 150));
		}
		else
		{
			// Fallback placeholder with helpful instruction
			ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "[Logo Image Missing]");
			ImGui::Text("Place 'logo.png' or 'dlsu_logo.png'");
			ImGui::Text("in the project root directory.");
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		
		if (ImGui::CollapsingHeader("About", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("Zero Systems Engine");
			ImGui::Text("Developed by: Arvin Dacanay");
			ImGui::Spacing();
			ImGui::Text("Acknowledgements:");
			ImGui::BulletText("PardCode Game Engine Tutorial");
			ImGui::BulletText("GDENG03 Course");
		}
		
		ImGui::End();
	}

	// 3. Scene Hierarchy Window
	if (m_showHierarchy)
	{
		ImGui::Begin("Scene Hierarchy", &m_showHierarchy);
		if (ImGui::Button(m_isPlaying ? "Edit Mode" : "Play Mode"))
		{
			setPlayMode(!m_isPlaying);
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset Physics Scene"))
		{
			resetPhysicsScene();
		}
		ImGui::Text("Mode: %s", m_isPlaying ? "Play" : "Edit");
		ImGui::Separator();
		
		auto drawSelectable = [this](const char* label, dx3d::GameObject* obj) {
			bool selected = (m_selectedObject == obj);
			if (ImGui::Selectable(label, selected))
			{
				m_selectedObject = obj;
			}
		};

		drawSelectable("Grid/Plane", m_plane);
		for (auto i = 0u; i < m_physicsCubes.size(); ++i)
		{
			char label[32]{};
			sprintf_s(label, "Physics Cube (%u)", i + 1u);
			drawSelectable(label, m_physicsCubes[i]);
		}
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
			else if (m_selectedObject == m_player) name = "Player / Main Camera";
			else
			{
				for (auto i = 0u; i < m_physicsCubes.size(); ++i)
				{
					if (m_selectedObject == m_physicsCubes[i])
					{
						static char cubeName[32]{};
						sprintf_s(cubeName, "Physics Cube (%u)", i + 1u);
						name = cubeName;
						break;
					}
				}
			}

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

	// 5. Color Picker Window (Placeholder with Hue Wheel)
	if (m_showColorPicker)
	{
		ImGui::Begin("Color Picker Screen", &m_showColorPicker, ImGuiWindowFlags_AlwaysAutoResize);
		static float color[3] = { 0.5f, 0.8f, 0.6f };
		ImGui::ColorPicker3("Color", color, ImGuiColorEditFlags_PickerHueWheel);
		ImGui::End();
	}
}
